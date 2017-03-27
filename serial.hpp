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

	template <typename R, typename E>
	void put (R& r, E e) {
		while (!e.empty()) {
			r.front() = e.front();
			r.popFront();
			e.popFront();
		}
	}

	template <typename R, typename E>
	void put (R&& r, const E e) { put<R, E>(r, e); }

	template <>
	void put<Slice, Slice> (Slice& r, const Slice e) {
		assert(r.size() >= e.size());
		memcpy(r.begin(), e.begin(), e.size());
		r.popFrontN(e.size());
	}

	template <typename E, bool BE = false>
	void putX (Slice& r, const E e) {
		using T = typename Slice::value_type;

		static_assert(sizeof(E) % sizeof(T) == 0);
		assert((sizeof(E) / sizeof(T)) <= r.size());

		auto ptr = reinterpret_cast<E*>(r.begin());
		*ptr = e;
		if (BE) reverseBytes(ptr);
		r.popFrontN(sizeof(E) / sizeof(T));
	}

	template <typename E, bool BE = false>
	void putX (Slice&& r, const E e) { put<E, BE>(r, e); }

	template <typename E, bool BE = false>
	auto read (Slice& r) {
		using T = typename Slice::value_type;

		const auto e = peek<E, BE>(r);
		r.popFrontN(sizeof(E) / sizeof(T));
		return e;
	}
}
