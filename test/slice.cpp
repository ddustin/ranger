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

	serial::put<uint32_t>(g, 19);
	const auto i = serial::peek<uint32_t>(g);
	assert(i == 19);

	g.popFrontN(30);
	assert(g.size() == e.size() - 30);

	Slice(e).assign(g);

	std::array<uint8_t, 32> array;
	Slice(e).assign(array);

	return 0;
}
