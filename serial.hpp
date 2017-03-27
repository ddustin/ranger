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
	template <typename E, bool BE = false>
	auto peek (const Slice& r) {
		using T = typename Slice::value_type;

		static_assert(sizeof(E) % sizeof(T) == 0);
		assert((sizeof(E) / sizeof(T)) <= r.size());

		auto value = *(reinterpret_cast<const E*>(r.begin()));
		if (BE) reverseBytes(&value);

		return value;
	}

	template <typename E, bool BE = false>
	auto read (Slice& r) {
		using T = typename Slice::value_type;

		const auto e = peek<E, BE>(r);
		r.popFrontN(sizeof(E) / sizeof(T));
		return e;
	}

	template <typename E, bool BE = false>
	void write (Slice& r, const E e) {
		using T = typename Slice::value_type;

		static_assert(sizeof(E) % sizeof(T) == 0);
		assert((sizeof(E) / sizeof(T)) <= r.size());

		auto ptr = reinterpret_cast<E*>(r.begin());
		*ptr = e;
		if (BE) reverseBytes(ptr);
		r.popFrontN(sizeof(E) / sizeof(T));
	}

	template <typename E, bool BE = false>
	void write (Slice&& r, const E e) { write<E, BE>(r, e); }
}
