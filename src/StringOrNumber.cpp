#include "StringOrNumber.hpp"

#include <stdexcept>

StringOrNumber::StringOrNumber()
	: type(FieldType::none)
{}

StringOrNumber::StringOrNumber(const char *s)
	: stringValue(s), numberValue(0), type(FieldType::string)
{}

StringOrNumber::StringOrNumber(int n)
	: stringValue(), numberValue(n), type(FieldType::number)
{}

FieldType StringOrNumber::getType() const
{
	return type;
}

std::string StringOrNumber::getString() const
{
	if (type == FieldType::string)
		return stringValue;
	else
		throw std::logic_error("No string value inside");
}

int StringOrNumber::getNumber() const
{
	if (type == FieldType::number)
		return numberValue;
	else
		throw std::logic_error("No number value inside");
}
