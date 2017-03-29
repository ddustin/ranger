#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <list>
#include <map>
#include <openssl/sha.h>
#include <type_traits>
#include <vector>

#include "../ranger.hpp"
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
}

void rangeTests2 () {
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

	const auto rrr2 = range(range(rrr));
	assert(rrr2.size() == ccc.size());
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

	retro(retro(retro(retro(retro(retro(yy)))))).put(range(xx));
	for (auto i = range(yy); !i.empty(); i.popBack()) {
		assert((i.back() + 1u) == i.size());
	}

// 	const auto zz = yy;
// 	memcpy(zz.begin(), xx.begin(), zz.size());

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

void serialTests () {
	std::array<uint8_t, 4> a = {165, 102, 42, 10};

	auto x = serial::peek<int32_t>(a);
	assert(x == ((a[0] << 0) + (a[1] << 8) + (a[2] << 16) + (a[3] << 24)));

	auto y = serial::peek<int32_t, true>(a);
	assert(y == ((a[0] << 24) + (a[1] << 16) + (a[2] << 8) + (a[3] << 0)));

	auto r = range(a);
	serial::read<int32_t>(r);
	assert(r.size() == 0);

	auto rr = range(a);
	assert(rr.size() == 4);
	serial::write<int16_t>(rr, 2048);
	assert(serial::peek<int16_t>(a) == 2048);
	assert(rr.size() == 2);

	assert(serial::read<int16_t>(range(a)) == 2048);
	serial::write<int16_t>(range(a), 420);
	assert(serial::peek<int16_t>(a) == 420);

	serial::put<int16_t>(a, 890);
	assert(serial::peek<int16_t>(a) == 890);

	uint32_t mantissa = 0x90ffccde;
	std::array<uint8_t, 4> expected = {};
	expected[3] = static_cast<uint8_t>(mantissa & 0xff);
	expected[3 - 1] = static_cast<uint8_t>(mantissa >> 8);
	expected[3 - 2] = static_cast<uint8_t>(mantissa >> 16);
	expected[3 - 3] = static_cast<uint8_t>(mantissa >> 24);
	const auto em = serial::peek<uint32_t, true>(expected);
	assert(em == mantissa);

	std::array<uint8_t, 4> actual = {};
	serial::put<uint32_t, true>(actual, mantissa);
	const auto am = serial::peek<uint32_t, true>(actual);
	assert(am == mantissa);

	assert(memcmp(expected.begin(), actual.begin(), actual.size()) == 0);
	printr(range(actual));
	printr(range(expected));
}

void otherUsageTests () {
	std::map<char, int> vs;
	vs['x'] = 3;
	vs['y'] = 44;
	vs['z'] = 555;

	auto rm = range(vs);
	for (auto i = rm; !i.empty(); i.popFront()) {
		printf("%c %i\n", i.front().first, i.front().second);
	}

	assert(rm[0].second == 3);
	assert(rm[1].second == 44);
	assert(rm[2].second == 555);
	// boom (fails on size assertion)
// 	assert(rm[4].second == 3);

	uint64_t h = 0xffff000011110000;
	std::list<uint8_t> ls;
	ls.push_back(0);
	ls.push_back(0);
	ls.push_back(0);
	ls.push_back(0);
	ls.push_back(0);
	ls.push_back(0);
	ls.push_back(0);
	ls.push_back(0);

	serial::put<uint64_t>(range(ls), h);
	// compare, byte for byte again h
	size_t i = 0;
	for (auto x = ls.begin(); x != ls.end(); ++x) {
		auto hp = reinterpret_cast<uint8_t*>(&h);
		assert(hp[i] == *x);
		++i;
	}
}

void overloadTests () {
// 	std::array<uint8_t, 10> x;
	std::vector<uint8_t> x(32, 0xff);
	auto xr = ptr_range(x);
	assert(xr.size() == 32);
	assert(xr.back() == 0xff);

	SHA256_CTX context;
	SHA256_Init(&context);
	SHA256_Update(&context, xr.begin(), xr.size());
	SHA256_Final(xr.begin(), &context);
	printr(xr);
}

int main () {
	rangeTests();
	rangeTests2();
	retroTests();
	serialTests();
	otherUsageTests();
	overloadTests();

	return 0;
}
