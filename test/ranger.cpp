#include <iostream>
#include <type_traits>

struct Foo {
	bool empty() { return true; }
	int front() { return 0; }
	void popFront() {}
};

struct Bar {
	char front() { return 'a'; }
	char operator[] (size_t) { return 'b'; }
	size_t length () const { return 5; }
};

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

// int main () {
// 	std::cout << "Foo isInputRange: " << isInputRange<Foo>() << std::endl;
// 	std::cout << "Bar isInputRange: " << isInputRange<Bar>() << std::endl;
// 	std::cout << "Foo hasLength : " << hasLength<Foo>::value << std::endl;
// 	std::cout << "Bar hasLength : " << hasLength<Bar>::value << std::endl;
//
// 	std::cout << "Bar length : " << lengthOf(Bar()) << std::endl;
//
// 	return 0;
// }
