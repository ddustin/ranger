#include <array>
#include <cassert>
#include <iostream>

#include "../slice.hpp"
#include "../serial.hpp"

int main () {
	StackSlice<32> a;
	a[10] = 16;
	a[20] = 63;
	static_assert(sizeof(a) == 32);

	const auto b = a.drop(10);
	assert(b.size() == 22);
	assert(b[0] == 16);
	assert(a.size() == 32);

	const auto c = b.drop(10);
	assert(c.size() == 12);
	assert(c[0] == 63);
	assert(b.size() == 22);
	assert(a.size() == 32);

	Slice d = a;
	d[0] = 95;
	d[d.size() - 2] = 54;
	d[d.size() - 1] = 17;
	assert(d.front() == 95);
	assert(d.back() == 17);
	d.back() = 11;
	assert(d.back() == 11);
	d.popBack();
	assert(d.back() == 54);

	d.popFrontN(10);
	assert(d[0] == 16);
	d.popFrontN(10);
	assert(d[0] == 63);
	assert(a.size() == 32);
	static_assert(sizeof(d) == sizeof(uint8_t*) * 2);

	HeapSlice e(60);
	assert(e.size() == 60);
	const auto f = e.drop(10);
	assert(f.size() == 50);
	assert(e.size() == 60);

	auto g = Slice(e);
	assert(e.size() == g.size());

	g.popFrontN(30);
	assert(g.size() == e.size() - 30);

	return 0;
}
