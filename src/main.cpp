#include <iostream>

#include "QueryLexer.hpp"

int main() {
	// Tuple t{{1, 2, 3, "ala", "ma", "kota"}};
	// t.print();
    //
	// auto raw = t.rawFormat();
    //
	// Tuple t2{raw.get()};
	// t2.print();

	auto pattern = "(integer:1, string:*, string:\"xy*\", integer:<=5)";
	QueryLexer ql{pattern};
	auto ret = ql.tokenize();

	for (auto &s : ret)
		std::cout << s << std::endl;
}
