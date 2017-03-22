#include <iostream>

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

#include "../ranger.hpp"

template <typename R>
typename std::enable_if<hasLength<R>::value, size_t>::type lengthOf (R r) {
	return r.length();
}

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
