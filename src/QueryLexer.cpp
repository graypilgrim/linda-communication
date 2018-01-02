#include "QueryLexer.hpp"

#include <iostream>

QueryLexer::QueryLexer(const std::string &pattern)
	: state(State::start), pattern(pattern)
{}

std::vector<std::string> QueryLexer::tokenize()
{
	std::vector<std::string> tokens;
	std::string token;
	for (auto &c : pattern) {
		switch (state)
		{
		case State::start:
			if (isOpeningParanthesis(c)) {
				tokens.emplace_back(1, c);
				break;
			} else if (isLetter(c)) {
				token.push_back(c);
				state = State::type;
				break;
			} else if (isWhitespace(c)) {
				break;
			} else {
				std::cout << "Invalid pattern, unexpected symbol: " << c << std::endl;
				return {};
			}

		case State::type:
			if (isLetter(c)) {
				token.push_back(c);
				break;
			} else if (isColon(c)) {
				tokens.push_back(token);
				token.clear();
				tokens.emplace_back(1, c);
				state = State::op;
				break;
			} else {
				std::cout << "Invalid pattern, unexpected symbol: " << c << std::endl;
				return {};
			}

		case State::op:
			if (isOperator(c)) {
				token.push_back(c);
				break;
			} else if (isDigit(c)) {
				tokens.push_back(token);
				token.clear();
				token.push_back(c);
				state = State::number;
				break;
			} else if (isAsterisk(c) || isQuotationMark(c)) {
				tokens.push_back(token);
				token.clear();
				token.push_back(c);
				state = State::string;
				break;
			} else {
				std::cout << "Invalid pattern, unexpected symbol: " << c << std::endl;
				return {};
			}

		case State::number:
			if (isDigit(c)) {
				token.push_back(c);
				break;
			} else if (isComa(c) || isClosingParanthesis(c)) {
				tokens.push_back(token);
				token.clear();
				tokens.emplace_back(1, c);
				state = State::start;
				break;
			} else {
				std::cout << "Invalid pattern, unexpected symbol: " << c << std::endl;
				return {};
			}

		case State::string:
			if (isAlNum(c) || isAsterisk(c) || isQuotationMark(c) || isWhitespace(c)) {
				token.push_back(c);
				break;
			} else if (isComa(c) || isClosingParanthesis(c)) {
				if (token.size() > 1 && !isQuotationMark(token[token.size() - 1])) {
					std::cout << "Invalid pattern, unexpected symbol: " << c << std::endl;
					return {};
				}
				tokens.push_back(token);
				token.clear();
				tokens.emplace_back(1, c);
				state = State::start;
				break;
			} else {
				std::cout << "Invalid pattern, unexpected symbol: " << c << std::endl;
				return {};
			}
		}
	}

	return tokens;
}

bool QueryLexer::isOpeningParanthesis(char c)
{
	return c == '(';
}

bool QueryLexer::isClosingParanthesis(char c)
{
	return c == ')';
}

bool QueryLexer::isDigit(char c)
{
	return c >= '0' && c <= '9';
}

bool QueryLexer::isLetter(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool QueryLexer::isAlNum(char c)
{
	return isDigit(c) || isLetter(c);
}

bool QueryLexer::isColon(char c)
{
	return c == ':';
}

bool QueryLexer::isAsterisk(char c)
{
	return c == '*';
}

bool QueryLexer::isComa(char c)
{
	return c == ',';
}

bool QueryLexer::isQuotationMark(char c)
{
	return c == '"';
}

bool QueryLexer::isOperator(char c)
{
	return c == '<' || c == '>' || c == '=';
}

bool QueryLexer::isWhitespace(char c)
{
	return c <= ' ';
}
