#include "QueryParser.hpp"

#include "QueryLexer.hpp"

#include <iostream>

QueryParser::QueryParser(const std::vector<std::string> &tokens)
	: tokens(tokens), currentFieldType(FieldType::none)
{}

QueryVec QueryParser::parse()
{
	it = tokens.begin();

	if (*it != "(") {
		std::cout << "Opening paranthesis expected" << std::endl;
		return {};
	}

	++it;
	type();

	return queries;
}

void QueryParser::type()
{
	if (*it == "integer")
		currentFieldType = FieldType::number;
	else if (*it == "string")
		currentFieldType = FieldType::string;

	if (currentFieldType == FieldType::none) {
		std::cout << "Invalid type: " << *it << std::endl;
		queries.clear();
		return;
	}

	++it;
	colon();
}

void QueryParser::colon()
{
	if (*it != ":") {
		std::cout << "Invalid symbol. Expected: colon, received: " << *it << std::endl;
		queries.clear();
		return;
	}

	++it;
	operators();
}

void QueryParser::operators()
{
	if (*it == "==" || *it == "")
		currentOperatorType = OperatorType::equal;
	else if (*it == ">=")
		currentOperatorType = OperatorType::greater_or_equal;
	else if (*it == ">")
		currentOperatorType = OperatorType::greater;
	else if (*it == "<=")
		currentOperatorType = OperatorType::less_or_equal;
	else if (*it == "<")
		currentOperatorType = OperatorType::less;
	else {
		std::cout << "Invalid symbol. Expected: operator, received: " << *it << std::endl;
		queries.clear();
		return;
	}

	++it;
	value();
}

void QueryParser::value()
{
	if (*it == "*" || *it == "\"*\"") {
		queries.emplace_back(currentFieldType, [](const StringOrNumber &son){return true;});
		++it;
	} else if (currentFieldType == FieldType::number) {
		number();
	} else {
		string();
	}

	if (*it == ",") {
		currentFieldType = FieldType::none;
		++it;
		type();
	} else if (*it == ")") {
		return;
	} else {
		std::cout << "Invalid symbol. Expected: coma or paranthesis, received: " << *it << std::endl;
		queries.clear();
		return;
	}
}

void QueryParser::number()
{
	int nm;
	try
	{
		nm = std::stoi(*it);
	}
	catch (std::exception e)
	{
		std::cout << "Error. Expected: number, received: " << *it << std::endl;
		queries.clear();
		return;
	}

	switch (currentOperatorType)
	{
	case OperatorType::equal:
		queries.emplace_back(currentFieldType, [nm](const StringOrNumber &son){
			if (son.getType() == FieldType::number)
				return son.getNumber() == nm;
			return false;});
		break;

	case OperatorType::greater_or_equal:
		queries.emplace_back(currentFieldType, [nm](const StringOrNumber &son){
			if (son.getType() == FieldType::number)
				return son.getNumber() >= nm;
			return false;}
		);
		break;

	case OperatorType::greater:
		queries.emplace_back(currentFieldType, [nm](const StringOrNumber &son){
			if (son.getType() == FieldType::number)
				return son.getNumber() > nm;
			return false;});
		break;

	case OperatorType::less_or_equal:
		queries.emplace_back(currentFieldType, [nm](const StringOrNumber &son){
			if (son.getType() == FieldType::number)
				return son.getNumber() <= nm;
			return false;});
		break;

	case OperatorType::less:
		queries.emplace_back(currentFieldType, [nm](const StringOrNumber &son){
			if (son.getType() == FieldType::number)
				return son.getNumber() < nm;
			return false;});
		break;
	}

	++it;
}

void QueryParser::string()
{
	if (it->size() < 2) {
		std::cout << "Error. Provided string has invalid format: " << *it << std::endl;
		queries.clear();
		return;
	}

	auto pattern = std::string(*it, 1, it->size() - 2);

	switch (currentOperatorType)
	{
	case OperatorType::equal:
		queries.emplace_back(currentFieldType, [pattern](const StringOrNumber &son){
			if (son.getType() == FieldType::string)
				return QueryParser::compareStrings(son.getString(), pattern) == 0;
			return false;
		});
		break;

	case OperatorType::greater_or_equal:
		queries.emplace_back(currentFieldType, [pattern](const StringOrNumber &son){
			if (son.getType() == FieldType::string)
				return QueryParser::compareStrings(son.getString(), pattern) >= 0;
			return false;
		});
		break;

	case OperatorType::greater:
		queries.emplace_back(currentFieldType, [pattern](const StringOrNumber &son){
			if (son.getType() == FieldType::string)
				return QueryParser::compareStrings(son.getString(), pattern) > 0;
			return false;
		});
		break;

	case OperatorType::less_or_equal:
		queries.emplace_back(currentFieldType, [pattern](const StringOrNumber &son){
			if (son.getType() == FieldType::string)
				return QueryParser::compareStrings(son.getString(), pattern) <= 0;
			return false;
		});
		break;

	case OperatorType::less:
		queries.emplace_back(currentFieldType, [pattern](const StringOrNumber &son){
			if (son.getType() == FieldType::string)
				return QueryParser::compareStrings(son.getString(), pattern) < 0;
			return false;
		});
		break;
	}

	++it;
}

int QueryParser::compareStrings(const std::string &str, const std::string &pattern)
{
	auto strIndex = 0u;
	auto patternIndex = 0u;

	while (patternIndex < pattern.size()) {
		if (pattern[patternIndex] == '*') {
			++patternIndex;

			if (patternIndex == pattern.size()) return 0;

			auto tempStrIndex = str.size() - (pattern.size() - patternIndex);
			if (tempStrIndex < strIndex) return 1;

			strIndex = tempStrIndex;
		}

		if (pattern[patternIndex] < str[strIndex])
			return -1;
		else if (pattern[patternIndex] > str[strIndex])
			return 1;

		++strIndex;
		++patternIndex;
	}

	return 0;
}
