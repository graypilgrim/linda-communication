#include <iostream>

#include "Tuple.hpp"

int main() {
	std::initializer_list<StringOrNumber> list{1, 2, 3, 4, 5, 6, "a", "a"};

	Tuple t{list};

	t.print();
}
