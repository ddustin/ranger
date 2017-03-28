#include <array>
#include <cassert>
#include <iostream>
#include <type_traits>
#include <vector>

#include "../slice.hpp"
#include "../serial.hpp"

// struct Foo {
// 	bool empty() { return true; }
// 	int front() { return 0; }
// 	void popFront() {}
// };
//
// struct Bar {
// 	char front() { return 'a'; }
// 	char operator[] (size_t) { return 'b'; }
// 	size_t length () const { return 5; }
// };

// has front, popFront, and empty
// template <typename R>
// struct isInputRange {
// 	using T = typename R::value_type;
//
// 	template <bool result =
// 		std::is_same<T, decltype(((R*)nullptr)->front())>::value &&
// 		std::is_same<bool, decltype(((R*)nullptr)->empty())>::value &&
// 		std::is_same<void, decltype(((R*)nullptr)->popFront())>::value>
// 	constexpr static bool eval (int) { return result; }
// 	constexpr static bool eval (...) { return false; }
//
// 	static const auto value = eval(0);
// };

// 	template <typename E>
// 	typename std::enable_if<isInputRange<E>::value, void>::type assign (E e) {
// 		static_assert(std::is_same<T, typename E::value_type>::value);
// 		assert(this->size() >= e.size());
//
// 		for (size_t i = 0; !e.empty(); e.popFront(), ++i) {
// 			(*this)[i] = e.front();
// 		}
// 	}

// #include "../ranger.hpp"
//
// template <typename R>
// typename std::enable_if<hasLength<R>::value, size_t>::type lengthOf (R r) {
// 	return r.length();
// }

int main () {
	std::array<uint8_t, 32> a;
	range(a).put(iota<uint8_t>(0, 32, 1));
	range(a).put(iota<uint8_t>(1, 33, 1));
	range(a).put(iota<uint8_t>(1, 64, 2));

// 	for (auto i = range(a); !i.empty(); i.popFront()) {
// 		printf("%i ", i.front());
// 	}

	auto b = range(a).drop(10);
	assert(a.size() == 32);
	assert(b.size() == 22);

	assert(b.front() == 21);
	assert(b.front() == b[0]);

	const auto c = b.drop(21);
	assert(a.size() == 32);
	assert(b.size() == 22);
	assert(c.size() == 1);
	assert(c[0] == 63);

	auto d = range(a);
	d[0] = 95;
	d[d.size() - 2] = 54;
	d[d.size() - 1] = 17;
	assert(d.front() == 95);
	d.front() = 96;
	assert(d.front() == 96);
	assert(d.back() == 17);
	d.back() = 11;
	assert(d.back() == 11);
	d.popBack();
	assert(d.back() == 54);

	d.popFrontN(20);
	assert(d[0] == a[20]);

	std::vector<uint8_t> e(60);
	assert(e.size() == 60);
	const auto f = range(e).drop(10);
	assert(f.size() == 50);
	assert(e.size() == 60);

	auto g = range(e);
	assert(e.size() == g.size());

	g.popFrontN(30);
	assert(g.size() == e.size() - 30);
	std::array<uint8_t, 4> xx;
	for (auto i = 0; i < 4; ++i) {
		xx[i] = static_cast<uint8_t>((i + 1) * 10);
	}

	std::array<uint8_t, 4> yy;
	std::array<uint8_t, 4> yy2;
	std::array<uint8_t, 4> zz;
	std::array<uint8_t, 4> zz2;

	range(yy).put(range(xx));
	retro(range(yy2)).put(retro(range(xx)));

	range(zz).put(retro(range(xx)));
	retro(range(zz2)).put(range(xx));

	for (auto i = 0; i < 4; ++i) {
		assert(yy[i] == xx[i]);
		assert(yy2[i] == xx[i]);
		assert(zz[i] == xx[3 - i]);
		assert(zz2[i] == xx[3 - i]);
	}

// 	std::cout << "Foo isInputRange: " << isInputRange<Foo>() << std::endl;
// 	std::cout << "Bar isInputRange: " << isInputRange<Bar>() << std::endl;
// 	std::cout << "Foo hasLength : " << hasLength<Foo>::value << std::endl;
// 	std::cout << "Bar hasLength : " << hasLength<Bar>::value << std::endl;
//
// 	std::cout << "Bar length : " << lengthOf(Bar()) << std::endl;
//
// 	return 0;

	return 0;
}
