#include <type_traits>

// template <typename R>
// struct hasBegin {
// 	using T = R::value_type;
//
// 	template <bool result = std::is_same<T*, decltype(((R*)nullptr)->begin())>::value>
// 	constexpr static bool eval (int) { return result; }
// 	constexpr static bool eval (...) { return false; }
//
// 	static constexpr const bool value = ::eval(0);
// };

// template <typename R, bool result = std::is_same<decltype(((R*)nullptr)->length()), size_t>::value>
// constexpr bool hasLengthImpl (int) { return result; }
//
// template <typename R>
// constexpr bool hasLengthImpl (...) { return false; }
//
// template <typename R>
// constexpr bool hasLength () { return ; }

// TR auto isInfinite (typename std::enable_if<std::is_same<decltype(R().length()), size_t>::value, void>::type) {
// 	return false;
// }
// TR bool isInfinite () {
// 	R range;
// 	return !range.empty();
// }

// TR auto isRandomAccessRange () {
// 	R range;
// 	return (hasLength<R>() || isInfinite<R>()) &&
// 		std::is_same<decltype(range.front()), decltype(range[0])>::value;
// }
