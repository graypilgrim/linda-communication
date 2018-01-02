#ifndef QUERY_PARSER_HPP
#define QUERY_PARSER_HPP

#include "utils.hpp"
#include "StringOrNumber.hpp"

#include <string>
#include <vector>
#include <utility>
#include <functional>

class QueryParser
{
public:
	using Query = std::pair<FieldType, std::function<bool(const StringOrNumber &son)>>;
	QueryParser(const std::vector<std::string> &tokens);
	std::vector<Query> parse();

private:
	std::vector<std::string> tokens;
};

#endif
