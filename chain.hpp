#include <algorithm>
#include <tuple>

namespace {
	template <typename R>
	auto _length (const R& range) const {
		return range.length();
	}

	template <typename R, typename ... Arguments>
	auto _length (const R& range, Arguments... args) const {
		return _length(range) + _length(args...);
	}
}

template <typename U, typename ... Arguments>
struct Chain {
private:
	std::tuple<U, Arguments...> ranges;
	size_t lengths[sizeof...(Arguments)];

	template <typename R>
	auto assign (const size_t i, const size_t accum, const R& range) {
		const auto value = _length(range);
		this->lengths[i] = value;
		return value;
	}

	template <typename R, typename ... Arguments>
	void assign (const size_t i, const size_t accum, const R& range, Arguments... args) {
		accum += this->assign(i, accum, range);
		this->assign(i + 1, accum, args...);
	}

public:
	Chain (const U& range, Arguments... args) : ranges(range, args...) {
		this->assign(0, 0, range, args...);
	}

	auto length () const {
		return std::apply(_length, this->ranges);
	}

	auto& operator[] (const size_t i) {
		return std::lower_bound(this->begin(), this->end(), key);

		assert(i < this->length());
		return this->begin()[i];
	}

	operator []
	slengths.lowerBound(i + 1);
}

template <typename ... Arguments>
auto chain (Arguments... args) {
	return Chain<Arguments...>(args...);
}
