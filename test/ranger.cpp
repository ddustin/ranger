#include <array>
#include <cassert>
#include <iostream>
#include <type_traits>
#include <vector>

#include "../slice.hpp"
#include "../serial.hpp"

template <typename R>
void printr (const R r) {
	for (auto i = r; !i.empty(); i.popFront()) {
		printf("%i ", i.front());
	}
	printf("\n");
}

void rangeTests () {
	std::array<uint8_t, 32> a;
	for (size_t i = 0; i < a.size(); ++i) a[i] = static_cast<uint8_t>((i * 2) + 1);

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

	auto h = range(g);
	assert(h.size() == g.size());

	auto i = range(range(g));
	assert(i.size() == g.size());

	const std::array<uint8_t, 4> ccc = {0, 1, 2, 3};
	for (auto i = range(ccc); !i.empty(); i.popBack()) {
		assert((i.back() + 1u) == i.size());
	}

	const auto rrr = range(ccc);
	for (auto i = rrr; !i.empty(); i.popBack()) {
		assert((i.back() + 1u) == i.size());
	}
	assert(rrr.size() == ccc.size());

	std::array<uint8_t, 4> yy;
	range(yy).put(range(ccc));
}

void retroTests () {
	std::array<uint8_t, 4> xx = {0, 1, 2, 3};
	std::array<uint8_t, 4> yy = {0};

	range(yy).put(retro(xx));
	for (auto i = range(yy); !i.empty(); i.popFront()) {
		assert((i.front() + 1u) == i.size());
	}

	yy = {0}; // reset
	retro(yy).put(range(xx));
	for (auto i = range(yy); !i.empty(); i.popFront()) {
		assert((i.front() + 1u) == i.size());
	}

	retro(retro(yy)).put(retro(xx));
	for (auto i = range(yy); !i.empty(); i.popFront()) {
		assert((i.front() + 1u) == i.size());
	}

	memcpy(yy.begin(), xx.begin(), yy.size());
	for (auto i = range(yy); !i.empty(); i.popBack()) {
		assert((i.back() + 1u) == i.size());
	}

	yy = {9,9,9,9}; // reset
	auto mm = retro(yy).drop(1);
	range(mm).put(range(xx).drop(1));

	assert(yy[0] == 3);
	assert(yy[1] == 2);
	assert(yy[2] == 1);
	assert(yy[3] == 9);

	yy = {9,9,9,9}; // reset
	auto nn = retro(range(yy).drop(1));
	range(nn).put(range(xx).drop(1));

	assert(yy[0] == 9);
	assert(yy[1] == 3);
	assert(yy[2] == 2);
	assert(yy[3] == 1);

	assert(retro(range(yy)).drop(2).size() == 2);
	assert(retro(range(yy)).drop(4).size() == 0);
}

int main () {
	rangeTests();
	retroTests();

	return 0;
}
