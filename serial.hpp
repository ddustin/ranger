// TODO: probaby unrelated to ranges, but
#pragma once

#include <algorithm>
#include <type_traits>

#include "endian.h"
#if __BYTE_ORDER != __LITTLE_ENDIAN
#error "big endian architecture not supported"
#endif

namespace {
	template <typename T>
	void reverseBytes (T* p) {
		auto q = reinterpret_cast<uint8_t*>(p);
		std::reverse(q, q + sizeof(T));
	}
}

namespace serial {
	template <typename E, bool BE = false, typename R>
	auto peek (const R& r) {
		using T = typename R::value_type;

		static_assert(std::is_same<T, uint8_t>::value);
		static_assert(sizeof(E) % sizeof(T) == 0);

		constexpr auto count = sizeof(E) / sizeof(T);
		assert(count <= r.size());

		E value;
		auto ptr = reinterpret_cast<T*>(&value);
		for (size_t i = 0; i < count; ++i) ptr[i] = r[i];

		if (BE) reverseBytes(&value);

		return value;
	}

	template <typename E, bool BE = false, typename R>
	void put (R& r, const E e) {
		using T = typename R::value_type;

		static_assert(std::is_same<T, uint8_t>::value);
		static_assert(sizeof(E) % sizeof(T) == 0);
		assert((sizeof(E) / sizeof(T)) <= r.size());

		auto ptr = reinterpret_cast<E*>(r.begin());
		*ptr = e;
		if (BE) reverseBytes(ptr);
	}

	template <typename E, bool BE = false, typename R>
	auto read (R& r) {
		using T = typename R::value_type;

		const auto e = peek<E, BE, R>(r);
		r.popFrontN(sizeof(E) / sizeof(T));
		return e;
	}

	template <typename E, bool BE = false, typename R>
	void write (R& r, const E e) {
		using T = typename R::value_type;

		put<E, BE, R>(r, e);
		r.popFrontN(sizeof(E) / sizeof(T));
	}

	// rvalue references wrappers
	template <typename E, bool BE = false, typename R> void put (R&& r, const E e) { put<E, BE, R>(r, e); }
	template <typename E, bool BE = false, typename R> auto read (R&& r) { return read<E, BE, R>(r); }
	template <typename E, bool BE = false, typename R> void write (R&& r, const E e) { write<E, BE, R>(r, e); }
}
