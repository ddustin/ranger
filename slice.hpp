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
	using iterator = typename std::conditional<std::is_const<R>::value, typename R::const_iterator, typename R::iterator>::type;
	using value_type = typename R::value_type;

private:
	iterator _begin;
	iterator _end;

public:
	Range (R& r) : _begin(r.begin()), _end(r.end()) {}

	auto begin () const { return this->_begin; }
	auto drop (size_t n) const { return ranger::drop(*this, n); }
	auto empty () const { return this->_begin == this->_end; }
	auto end () const { return this->_end; }
	auto size () const { return static_cast<size_t>(this->_end - this->_begin); }
	auto take (size_t n) const { return ranger::take(*this, n); }
	auto& back () { return *(_end - 1); }
	auto& front () { return *_begin; }
	void popBack () { this->popBackN(1); }
	void popBackN (size_t n) {
		assert(n <= this->size());
		this->_end -= n;
	}
	void popFront () { this->popFrontN(1); }
	void popFrontN (size_t n) {
		assert(n <= this->size());
		this->_begin += n;
	}

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
};

template <typename R>
struct RetroRange {
	using value_type = typename R::value_type;

private:
	R& _r;

public:
	RetroRange (R& r) : _r(r) {}

	auto drop (size_t n) const { return ranger::drop(*this, n); }
	auto empty () const { return this->_r.empty(); }
	auto size () const { return this->_r.size(); }
	auto take (size_t n) const { return ranger::take(*this, n); }
	auto& back () { return this->_r.front(); }
	auto& front () { return this->_r.back(); }
	void popBack () { this->popBackN(1); }
	void popBackN (size_t n) { this->_r.popFrontN(n); }
	void popFront () { this->popFrontN(1); }
	void popFrontN (size_t n) { this->_r.popBackN(n); }

	template <typename E>
	void put (E e) { return ranger::put(*this, e); }

	auto& operator[] (const size_t i) {
		return this->_r[this->size() - 1 - i];
	}

	auto operator[] (const size_t i) const {
		return this->_r[this->size() - 1 - i];
	}
};

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
