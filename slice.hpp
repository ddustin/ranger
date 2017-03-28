// TODO: range-ify
#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <memory>

namespace ranger {
	template <typename R>
	auto drop (R r, const size_t n) {
		r.popFrontN(n);
		return r;
	}

	// TODO: specialization for ranges without .size()
	template <typename R>
	auto take (R r, const size_t n) {
		r.popBackN(r.size() - n);
		return r;
	}

	template <typename R, typename E>
	void put (R& r, E e) {
		while (!e.empty()) {
			r.front() = e.front();
			r.popFront();
			e.popFront();
		}
	}

	// TODO
// 	template <typename R>
// 	void put <R, typename R::value_type> (R& r, typename R::value_type e) {
// 		r.front() = e;
// 		r.popFront();
// 	}
}

template <typename R>
struct Range {
	using iterator = typename R::iterator;
	using value_type = typename R::value_type;

private:
	iterator _begin;
	iterator _end;

public:
	Range (R& r) : _begin(r.begin()), _end(r.end()) {}

	auto begin () { return this->_begin; }
	auto end () { return this->_end; }
	auto begin () const { return this->_begin; }
	auto end () const { return this->_end; }
	auto empty () const { return this->_begin == this->_end; }
	auto size () const { return static_cast<size_t>(this->_end - this->_begin); }

	auto& front () { return *_begin; }
	auto& back () { return *(_end - 1); }

	auto drop (size_t n) const { return ranger::drop(*this, n); }
	auto take (size_t n) const { return ranger::take(*this, n); }

	template <typename E>
	void put (E e) { return ranger::put(*this, e); }

	auto& operator[] (const size_t i) {
		assert(i < this->size());
		return *(this->_begin + i);
	}

	auto operator[] (const size_t i) const {
		assert(i < this->size());
		return *(this->_begin + i);
	}

	template <typename E>
	auto operator< (const E& rhs) const {
		return std::lexicographical_compare(this->begin(), this->end(), rhs.begin(), rhs.end());
	}

	void popBackN (size_t n) {
		assert(n <= this->size());
		this->_end -= n;
	}

	void popFrontN (size_t n) {
		assert(n <= this->size());
		this->_begin += n;
	}

	void popBack () { this->popBackN(1); }
	void popFront () { this->popFrontN(1); }
};

template <typename R>
struct RetroRange {
private:
	R _r;

public:
	using value_type = typename R::value_type;
	RetroRange (const R r) : _r(r) {}

	// aliases
	auto empty () const { return this->_r.empty(); }
	auto size () const { return this->_r.size(); }
	auto& front () { return this->_r.back(); }
	auto& back () { return this->_r.front(); }

	auto drop (size_t n) const { return ranger::drop(*this, n); }
	auto take (size_t n) const { return ranger::take(*this, n); }

	template <typename E>
	void put (E e) { return ranger::put(*this, e); }

	auto& operator[] (const size_t i) {
		assert(i < this->size());
		return this->_r[this->size() - 1 - i];
	}

	auto operator[] (const size_t i) const {
		assert(i < this->size());
		return this->_r[this->size() - 1 - i];
	}

	void popBackN (size_t n) {
		assert(n <= this->size());
		this->_r.popFrontN(n);
	}

	void popFrontN (size_t n) {
		assert(n <= this->size());
		this->_r.popBackN(n);
	}

	void popBack () { this->popBackN(1); }
	void popFront () { this->popFrontN(1); }
};

// TODO: generic sequence
template <typename T>
struct Iota {
private:
	T _current;
	T _pastLast;
	T _step;

public:
	Iota (T current, T pastLast, T step = 1) {
		if ((current < pastLast && step >= 0) || (current > pastLast && step <= 0)) {
			this->_current = current;
			this->_pastLast = pastLast;
			this->_step = step;

			if (step > 0) {
				assert(this->size() <= std::numeric_limits<size_t>::max());

				this->_pastLast = static_cast<T>(pastLast - 1);
				this->_pastLast = static_cast<T>(this->_pastLast - ((this->_pastLast - current) % step));
			} else {
				assert(this->size() <= std::numeric_limits<size_t>::max());

				this->_pastLast = static_cast<T>(pastLast + 1);
				this->_pastLast = static_cast<T>(this->_pastLast + ((current - this->_pastLast) % -step));
			}

			this->_pastLast = static_cast<T>(this->_pastLast + step);

		// empty range
		} else {
			this->_current = this->_pastLast = current;
			this->_step = 1;
		}
	}

	auto empty () const { return this->_current == this->_pastLast; }
	auto& front () {
		assert(!this->empty());
		return this->_current;
	}

	auto& back () {
		assert(!this->empty());
		return this->_pastLast - this->_step;
	}

	void popBackN (size_t n) {
		assert(n <= this->size());
		this->_pastLast = static_cast<T>(this->_pastLast - (this->_step * n));
	}

	void popFrontN (size_t n) {
		assert(n <= this->size());
		this->_current = static_cast<T>(this->_current + (this->_step * n));
	}

	void popBack () { this->popBackN(1); }
	void popFront () { this->popFrontN(1); }

	auto drop (size_t n) const { return ranger::drop(*this, n); }
	auto take (size_t n) const { return ranger::take(*this, n); }

	size_t size () const {
		if (this->_step > 0) {
			return static_cast<size_t>((this->_pastLast - this->_current) / this->_step);
		} else {
			return static_cast<size_t>((this->_current - this->_pastLast) / -this->_step);
		}
	}
};

template <typename T>
auto iota (T current, T pastLast, T step) {
	return Iota<T>(current, pastLast, step);
}

template <typename R>
auto retro (R& r) {
	return RetroRange<R>(r);
}

template <typename R>
auto retro (R&& r) {
	return retro<R>(r);
}

template <typename R>
auto range (R& r) {
	return Range<R>(r);
}

template <typename R>
auto range (R&& r) {
	return range<R>(r);
}

// namespace ranger {
// 	template <>
// 	void put <Slice, Slice> (Slice& r, const Slice e) {
// 		assert(r.size() >= e.size());
// 		memcpy(r.begin(), e.begin(), e.size());
// 		r.popFrontN(e.size());
// 	}
// }
