#include "StringOrNumber.hpp"

#include <stdexcept>

StringOrNumber::StringOrNumber()
	: type(Type::empty)
{}

StringOrNumber::StringOrNumber(const char *s)
	: stringValue(s), numberValue(0), type(Type::string)
{}

StringOrNumber::StringOrNumber(int n)
	: stringValue(), numberValue(n), type(Type::number)
{}

StringOrNumber::Type StringOrNumber::getType()
{
	return type;
}

std::string StringOrNumber::getString()
{
	if (type == Type::string)
		return stringValue;
	else
		throw std::logic_error("No string value inside");
}

int StringOrNumber::getNumber()
{
	if (type == Type::number)
		return numberValue;
	else
		throw std::logic_error("No number value inside");
}
