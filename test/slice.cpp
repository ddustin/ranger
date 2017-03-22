#include <cassert>
#include <iostream>

#include "../slice.hpp"

int main () {
	StackSlice<32> a;
	a[10] = 16;
	a[20] = 63;
	static_assert(sizeof(a) == 32);

	const auto b = a.drop(10);
	assert(b.length() == 22);
	assert(b[0] == 16);

	const auto c = b.drop(10);
	assert(c.length() == 12);
	assert(c[0] == 63);

	Slice d = a;
	d.popFrontN(10);
	assert(d[0] == 16);
	d.popFrontN(10);
	assert(d[0] == 63);
	assert(a.length() == 32);
	static_assert(sizeof(d) == sizeof(uint8_t*) * 2);

	return 0;
}
