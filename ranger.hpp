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

template <typename I>
struct Range {
public:
	using iterator = I;
	using const_iterator = typename std::add_const<I>::type; // re-evaluate, this is [probably] not safe __at_all__
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using value_type = typename std::remove_reference<decltype(*I())>::type;

private:
	iterator _begin;
	iterator _end;

public:
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
auto range (R& r) {
	using iterator = typename std::conditional<
		std::is_const<R>::value,
		typename R::const_iterator,
		typename R::iterator
	>::type;

	return Range<iterator>(r.begin(), r.end());
}

template <typename R>
auto retro (R& r) {
	using reverse_iterator = typename std::conditional<
		std::is_const<R>::value,
		typename R::const_reverse_iterator,
		typename R::reverse_iterator
	>::type;

	return Range<reverse_iterator>(reverse_iterator(r.end()), reverse_iterator(r.begin()));
}

// rvalue references wrappers
template <typename R> auto range (R&& r) { return range<R>(r); }
template <typename R> auto retro (R&& r) { return retro<R>(r); }
