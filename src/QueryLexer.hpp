#ifndef QUERY_LEXER_HPP
#define QUERY_LEXER_HPP

#include <string>
#include <vector>

class QueryLexer
{
public:
	enum class State
	{
		start,
		type,
		op,
		number,
		string
	};

	QueryLexer(const std::string &pattern);
	std::vector<std::string> tokenize();

private:
	bool isOpeningParanthesis(char c);
	bool isClosingParanthesis(char c);
	bool isDigit(char c);
	bool isLetter(char c);
	bool isAlNum(char c);
	bool isColon(char c);
	bool isAsterisk(char c);
	bool isComa(char c);
	bool isQuotationMark(char c);
	bool isOperator(char c);
	bool isWhitespace(char c);

	State state;
	std::string pattern;
};

#endif
