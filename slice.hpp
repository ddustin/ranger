// TODO: range-ify
#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>

#include "endian.h"
#if __BYTE_ORDER != __LITTLE_ENDIAN
#error BIG_ENDIAN systems unsupported
#endif

namespace {
	template <typename T>
	void swapEndian (T* p) {
		auto q = reinterpret_cast<uint8_t*>(p);
		std::reverse(q, q + sizeof(T));
	}
}

#define SLICE_MIXIN_IMPL \
	template <typename Y = T> \
	auto peek (const size_t offset = 0) const { \
		static_assert(sizeof(Y) % sizeof(T) == 0); \
		assert(offset + (sizeof(Y) / sizeof(T)) <= this->length()); \
		return *(reinterpret_cast<Y*>(this->begin() + offset)); \
	} \
	template <typename Y, bool bigEndian = false> \
	void put (const Y value, size_t offset = 0) { \
		static_assert(sizeof(Y) % sizeof(T) == 0); \
		assert(offset + (sizeof(Y) / sizeof(T)) <= this->length()); \
		auto ptr = reinterpret_cast<Y*>(this->begin() + offset); \
		*ptr = value; \
		if (bigEndian) swapEndian(ptr); \
	} \
	auto& operator[] (const size_t i) { \
		assert(i < this->length()); \
		return this->begin()[i]; \
	} \
	auto operator[] (const size_t i) const { \
		assert(i < this->length()); \
		return this->begin()[i]; \
	} \
	template <typename R> \
	auto operator< (const R& rhs) const { \
		return std::lexicographical_compare(this->begin(), this->end(), rhs.begin(), rhs.end()); \
	}

template <typename T>
struct TypedSlice {
private:
	T* _begin;
	T* _end;

public:
	TypedSlice () : _begin(nullptr), _end(nullptr) {}
	TypedSlice (T* begin, T* end) : _begin(begin), _end(end) {
		assert(this->_begin);
		assert(this->_end);
	}

	auto begin () const { return this->_begin; }
	auto end () const { return this->_end; }
	auto length () const {
		return static_cast<size_t>(this->_end - this->_begin);
	}

	SLICE_MIXIN_IMPL

	auto drop (size_t n) const {
		assert(n <= this->length());
		return TypedSlice<T>(this->begin() + n, this->end());
	}

	auto take (size_t n) const {
		assert(n <= this->length());
		return TypedSlice<T>(this->begin(), this->begin() + n);
	}

	void popFrontN (size_t n) {
		assert(n <= this->length());
		this->_begin += n;
	}

	void popFront () {
		this->popFrontN(1);
	}

	template <typename Y>
	auto read () {
		static_assert(sizeof(Y) % sizeof(T) == 0);

		const auto value = this->template peek<Y>();
		this->popFrontN(sizeof(Y) / sizeof(T));
		return value;
	}

	auto readN (const size_t n) {
		const auto slice = this->take(n);
		this->popFrontN(n);
		return slice;
	}

	template <typename Y, bool bigEndian = false>
	void write (const Y value) {
		static_assert(sizeof(Y) % sizeof(T) == 0);

		this->template put<Y, bigEndian>(value);
		this->popFrontN(sizeof(Y) / sizeof(T));
	}

	// TODO: use std::copy / reverse iterators etc
	void writeN (const T* data, size_t n) {
		assert(n <= this->length());
		memcpy(this->begin(), data, n);
		this->popFrontN(n);
	}

	void writeNReverse (const T* data, size_t n) {
		assert(n <= this->length());

		for (size_t i = 0; i < n; ++i) {
			(*this)[i] = data[n - 1 - i];
		}

		this->popFrontN(n);
	}
};

template <typename T, size_t N>
struct TypedStackSlice {
private:
	T data[N];

public:
	auto begin () const { return this->data; }
	auto end () const { return this->data + N; }

	auto length () const { return N; };
	auto drop (size_t n) const { return TypedSlice<T>(this->data, this->data + N).drop(n); }
	auto take (size_t n) const { return TypedSlice<T>(this->data, this->data + N).take(n); }

	SLICE_MIXIN_IMPL
};

template <typename T>
struct TypedHeapSlice {
private:
	T const* _begin;
	T const* _end;

public:
	TypedHeapSlice (const size_t n) {
		this->_begin = new T[n];
		this->_end = this->_begin + n;
	}

	~TypedHeapSlice () {
		delete[] this->_begin;
	}

	TypedHeapSlice (const TypedHeapSlice&) = delete;

	auto begin () const { return this->_begin; }
	auto end () const { return this->_end; }

	auto length () const {
		return static_cast<size_t>(this->_end - this->_begin);
	}
	auto drop (size_t n) const { return TypedSlice<T>(this->_begin, this->_end).drop(n); }
	auto take (size_t n) const { return TypedSlice<T>(this->_begin, this->_end).take(n); }

	SLICE_MIXIN_IMPL
};

typedef TypedHeapSlice<uint8_t> HeapSlice;

template <size_t N>
using StackSlice = TypedStackSlice<uint8_t, N>;

typedef TypedSlice<uint8_t> Slice;

#undef SLICE_MIXIN_IMPL
