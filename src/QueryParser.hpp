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
	void type();
	void colon();
	void operators();
	void value();
	void number();
	void string();
	static int compareStrings(const std::string &token, const std::string &pattern);


	enum class OperatorType {equal, greater_or_equal, greater, less_or_equal, less};
	std::vector<std::string> tokens;
	std::vector<std::string>::iterator it;
	std::vector<Query> queries;
	FieldType currentFieldType;
	OperatorType currentOperatorType;
};

#endif
