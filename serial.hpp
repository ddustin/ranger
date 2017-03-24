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
	template <typename R, typename E, bool BE = false>
	auto peek (const R& range) {
		using T = typename R::value_type;

		static_assert(sizeof(E) % sizeof(T) == 0);
		assert((sizeof(E) / sizeof(T)) <= range.size());

		auto value = *(reinterpret_cast<const E*>(range.begin()));
		if (BE) reverseBytes(&value);

		return value;
	}

	template <typename R, typename E, bool BE = false>
	void put (R& r, const E e) {
		using T = typename R::value_type;

		static_assert(sizeof(E) % sizeof(T) == 0);
		assert((sizeof(E) / sizeof(T)) <= r.size());

		auto ptr = reinterpret_cast<E*>(r.begin());
		*ptr = e;
		if (BE) reverseBytes(ptr);
	}

	template <typename R, typename E, bool BE = false>
	auto read (R& r) {
		using T = typename R::value_type;

		const auto e = peek<R, E, BE>(r);
		r.popFrontN(sizeof(E) / sizeof(T));
		return e;
	}

	template <typename R, typename E, bool BE = false>
	void write (R& r, const E e) {
		using T = typename R::value_type;

		put<R, E, BE>(r, e);
		r.popFrontN(sizeof(E) / sizeof(T));
	}
}
