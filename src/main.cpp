#include <iostream>

#include "QueryLexer.hpp"
#include "QueryParser.hpp"

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
	auto tokens = ql.tokenize();

	for (auto &s : tokens)
		std::cout << s << std::endl;

	QueryParser qp{tokens};
	auto queries = qp.parse();

	std::cout << "queries.size: " << queries.size() << std::endl;

	std::cout << "integer: 4, res: " << queries[0].second(1) << std::endl;
	std::cout << "integer: 4, res: " << queries[1].second("mango") << std::endl;
	std::cout << "integer: 4, res: " << queries[2].second("xz") << std::endl;
	std::cout << "integer: 4, res: " << queries[3].second(6) << std::endl;
}
