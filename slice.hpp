// TODO: range-ify
#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <memory>
#include <type_traits>

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

	auto front () { return (*this)[0]; }
	auto back () { return (*this)[this->size() - 1]; }

	auto drop (size_t n) const {
		assert(n <= this->size());
		return TypedSlice<T>(this->begin() + n, this->end());
	}

	auto take (size_t n) const {
		assert(n <= this->size());
		return TypedSlice<T>(this->begin(), this->begin() + n);
	}

	template <typename E>
	void assign (E e) {
		static_assert(std::is_same<T, typename E::value_type>::value);
		assert(this->size() >= e.size());

		for (size_t i = 0; !e.empty(); e.popFront(), ++i) {
			(*this)[i] = e.front();
		}
	}

	void assign (const TypedSlice<T>& e) {
		assert(this->size() >= e.size());
		memcpy(this->begin(), e.begin(), e.size());
	}

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

	void popFrontN (size_t n) {
		assert(n <= this->size());
		this->_begin += n;
	}

	void popFront () {
		this->popFrontN(1);
	}
};

template <typename T, size_t N>
struct TypedStackSlice {
private:
	T data[N];

public:
	using value_type = T;

	auto begin () { return this->data; }
	auto end () { return this->data + N; }
	auto begin () const { return this->data; }
	auto end () const { return this->data + N; }
	auto empty () const { return N > 0; }
	auto size () const { return N; };

	auto front () { return (*this)[0]; }
	auto back () { return (*this)[N - 1]; }

	auto drop (size_t m) { return TypedSlice<T>(this->begin(), this->end()).drop(m); }
	auto take (size_t m) { return TypedSlice<T>(this->begin(), this->end()).take(m); }

	auto& operator[] (const size_t i) {
		assert(i < N);
		return this->data[i];
	}

	auto operator[] (const size_t i) const {
		assert(i < N);
		return this->data[i];
	}

	template <typename R>
	auto operator< (const R& rhs) const {
		return std::lexicographical_compare(this->begin(), this->end(), rhs.begin(), rhs.end());
	}
};

template <typename T>
struct TypedHeapSlice {
private:
	std::unique_ptr<T[]> _data;
	size_t n;

public:
	using value_type = T;

	TypedHeapSlice (const size_t n) : _data(new T[n]), n(n) {}

	auto begin () { return this->_data.get(); }
	auto end () { return this->_data.get() + this->n; }
	auto begin () const { return this->_data.get(); }
	auto end () const { return this->_data.get() + this->n; }
	auto empty () const { return this->n > 0; }
	auto size () const { return this->n; }

	auto front () { return (*this)[0]; }
	auto back () { return (*this)[this->n - 1]; }

	auto drop (size_t m) { return TypedSlice<T>(this->begin(), this->end()).drop(m); }
	auto take (size_t m) { return TypedSlice<T>(this->begin(), this->end()).take(m); }

	auto& operator[] (const size_t i) {
		assert(i < this->n);
		return this->_data[i];
	}

	auto operator[] (const size_t i) const {
		assert(i < this->n);
		return this->_data[i];
	}

	template <typename R>
	auto operator< (const R& rhs) const {
		return std::lexicographical_compare(this->begin(), this->end(), rhs.begin(), rhs.end());
	}
};

typedef TypedHeapSlice<uint8_t> HeapSlice;
typedef TypedSlice<uint8_t> Slice;
template <size_t N>
using StackSlice = TypedStackSlice<uint8_t, N>;
