# ranger
A modern, header-only, range library for C++.

### Work In Progress
Inspiration/motivation from ranges in [D](https://dlang.org/phobos/std_range.html).

## Example
None of these examples work... yet.

``` cpp
#include <ranger/ranger.hpp>

// ...

std::vector<int> numbers = {1, 2, 3};

for (auto &x : ranger::reversed(numbers)) {
	std::cout << x << ' ';
}
// 3, 2, 1

auto a = ranger::slice(numbers, 0, 2); // {1, 2}
a[1] = 8;

numbers.push_back(9);

auto b = ranger::slice(numbers).drop(1); // {2, 8, 9}
auto c = ranger::reversed(b); // {9, 8, 2}

auto d = ranger::memory_slice(numbers);
```


## LICENSE [ISC](LICENSE)
