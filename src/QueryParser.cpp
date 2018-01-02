#include "QueryParser.hpp"

#include "QueryLexer.hpp"

QueryParser::QueryParser(const std::vector<std::string> &tokens)
	: tokens(tokens)
{}

std::vector<QueryParser::Query> QueryParser::parse()
{

}
