#include "QueryParser.hpp"

#include "QueryLexer.hpp"

#include <iostream>

QueryParser::QueryParser(const std::vector<std::string> &tokens)
	: tokens(tokens), currentFieldType(FieldType::none)
{}

std::vector<QueryParser::Query> QueryParser::parse()
{
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
	if (*it == "*") {
		queries.emplace_back(currentFieldType, [](const StringOrNumber &son){return true;});
		return;
	}

	if (currentFieldType == FieldType::number) {
		number();
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
}
