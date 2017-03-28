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

template <typename R, typename I>
struct Range {
private:
	static constexpr auto IS_CONST = std::is_const<R>::value;
	static constexpr auto IS_REVERSED =
		std::is_same<I, typename R::reverse_iterator>::value ||
		std::is_same<I, typename R::const_reverse_iterator>::value;

public:
	using iterator = I;
	using const_iterator = typename std::conditional<
		IS_CONST,
		I,
		typename std::conditional<IS_REVERSED, typename R::const_reverse_iterator, typename R::const_iterator>::type
	>::type;

	using reverse_iterator = typename std::conditional<
		IS_CONST,
		typename std::conditional<IS_REVERSED, typename R::const_iterator, typename R::const_reverse_iterator>::type,
		typename std::conditional<IS_REVERSED, typename R::iterator, typename R::reverse_iterator>::type
	>::type;
	using const_reverse_iterator = typename std::conditional<
		IS_CONST,
		reverse_iterator,
		typename std::conditional<IS_REVERSED, typename R::const_iterator, typename R::const_reverse_iterator>::type
	>::type;

	using value_type = typename R::value_type;

private:
	iterator _begin;
	iterator _end;

public:
	Range (R& r) : _begin(r.begin()), _end(r.end()) {}
	Range (iterator begin, iterator end) : _begin(begin), _end(end) {}

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
auto retro (R& r) {
// TODO: why not?
// 	using reverse_iterator = typename std::conditional<
// 		std::is_const<R>::value,
// 		typename R::const_reverse_iterator,
// 		typename R::reverse_iterator
// 	>::type;
	using iterator = typename std::conditional<
		std::is_const<R>::value,
		typename R::const_iterator,
		typename R::iterator
	>::type;
	using reverse_iterator = std::reverse_iterator<iterator>;

	return Range<R, reverse_iterator>(reverse_iterator(r.end()), reverse_iterator(r.begin()));
}

template <typename R>
auto range (R& r) {
	using iterator = typename std::conditional<
		std::is_const<R>::value,
		typename R::const_iterator,
		typename R::iterator
	>::type;

	return Range<R, iterator>(r.begin(), r.end());
}

// rvalue references wrappers
template <typename R> auto retro (R&& r) { return retro<R>(r); }
template <typename R> auto range (R&& r) { return range<R>(r); }
