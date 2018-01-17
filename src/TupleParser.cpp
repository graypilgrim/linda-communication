#include "TupleParser.hpp"

#include <iostream>
#include <sstream>

namespace {
	enum class State
	{
		start,
		field,
		number,
		string
	};
}

TupleParser::TupleParser(const std::string &tuple)
	: stringTuple(tuple)
{}

Tuple TupleParser::parse()
{
	std::vector<StringOrNumber> ret;
	State state = State::start;
	std::string token;

	for (auto &c : stringTuple) {
		switch (state)
		{
		case State::start:
			if (c == '(') {
				state = State::field;
				break;
			}

			if (isWhitespace(c) || isOpeningParanthesis(c)) break;

			std::cout << "Expected opening paranthesis, received: " << c << std::endl;
			return {};

		case State::field:
			if (isWhitespace(c)) break;

			if (isDigit(c)) {
				state = State::number;
				token.push_back(c);
				break;
			}

			if (isQuotationMark(c)) {
				state = State::string;
				break;
			}

			if (isComa(c) || isClosingParanthesis(c)) break;

			std::cout << "Number, coma or quotation mark expected, received: " << c << std::endl;
			return {};

		case State::number:
			if (isDigit(c)) {
				token.push_back(c);
				break;
			}

			if (isComa(c) || isWhitespace(c) || isClosingParanthesis(c)) {
				std::stringstream ss;
				ss << token;
				int n;
				ss >> n;
				ret.emplace_back(n);
				token.clear();
				state = State::field;
				break;
			}

			std::cout << "Digit or coma expected, received: " << c << std::endl;
			return {};

		case State::string:
			if (isAlNum(c) || isWhitespace(c)) {
				token.push_back(c);
				break;
			}

			if (isQuotationMark(c)) {
				ret.emplace_back(token.c_str());
				token.clear();
				state = State::field;
				break;
			}

			std::cout << "Alnum sign or quotation mark expected, received: " << c << std::endl;
			return {};
		}
	}

	return Tuple{ret};
}

bool TupleParser::isDigit(char c)
{
	return c >= '0' && c <= '9';
}

bool TupleParser::isQuotationMark(char c)
{
	return c == '"';
}

bool TupleParser::isComa(char c)
{
	return c == ',';
}

bool TupleParser::isWhitespace(char c)
{
	return c <= ' ';
}

bool TupleParser::isLetter(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool TupleParser::isAlNum(char c)
{
	return isDigit(c) || isLetter(c);
}

bool TupleParser::isOpeningParanthesis(char c)
{
	return c == '(';
}

bool TupleParser::isClosingParanthesis(char c)
{
	return c == ')';
}
