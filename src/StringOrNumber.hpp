#ifndef STRING_OR_NUMBER_HPP
#define STRING_OR_NUMBER_HPP

#include "utils.hpp"

#include <string>

class StringOrNumber
{
public:
	StringOrNumber();
	StringOrNumber(const char *s);
	StringOrNumber(int n);

	FieldType getType() const;
	std::string getString() const;
	int getNumber() const;

private:
	std::string stringValue;
	int numberValue;
	FieldType type;
};

#endif
