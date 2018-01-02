#include <iostream>

#include "Tuple.hpp"

int main() {
	Tuple t{{1, 2, 3, "ala", "ma", "kota"}};
	t.print();

	auto raw = t.rawFormat();

	Tuple t2{raw.get()};
	t2.print();
}
