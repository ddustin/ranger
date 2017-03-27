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

template <typename T>
struct TypedSlice {
private:
	T* _begin;
	T* _end;

public:
	using value_type = T;

	TypedSlice () : _begin(nullptr), _end(nullptr) {}
	TypedSlice (T* begin, T* end) : _begin(begin), _end(end) {
		assert(this->_begin != nullptr);
		assert(this->_end != nullptr);
	}
	template <typename R>
	TypedSlice (R& range) : _begin(range.begin()), _end(range.end()) {}

	auto begin () { return this->_begin; }
	auto end () { return this->_end; }
	auto begin () const { return this->_begin; }
	auto end () const { return this->_end; }
	auto empty () const { return this->_begin == this->_end; }
	auto size () const { return static_cast<size_t>(this->_end - this->_begin); }

	auto& front () { return (*this)[0]; }
	auto& back () { return (*this)[this->size() - 1]; }

	auto drop (size_t n) const { return ranger::drop(*this, n); }
	auto take (size_t n) const { return ranger::take(*this, n); }

	template <typename E>
	void put (E e) { return ranger::put(*this, e); }

	auto& operator[] (const size_t i) {
		assert(i < this->size());
		return this->_begin[i];
	}

	auto operator[] (const size_t i) const {
		assert(i < this->size());
		return this->_begin[i];
	}

	template <typename R>
	auto operator< (const R& rhs) const {
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

typedef TypedSlice<uint8_t> Slice;

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

template <typename R>
auto retro (const R r) {
	return RetroRange<R>(r);
}

namespace ranger {
	template <>
	void put <Slice, Slice> (Slice& r, const Slice e) {
		assert(r.size() >= e.size());
		memcpy(r.begin(), e.begin(), e.size());
		r.popFrontN(e.size());
	}
}
