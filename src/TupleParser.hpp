#ifndef TUPPLE_PARSER
#define TUPPLE_PARSER

#include "Tuple.hpp"

#include <string>

class TupleParser
{
public:
	TupleParser(const std::string &tuple);
	Tuple parse();

private:
	bool isDigit(char c);
	bool isQuotationMark(char c);
	bool isComa(char c);
	bool isWhitespace(char c);
	bool isLetter(char c);
	bool isAlNum(char c);

	std::string stringTuple;
};

#endif
