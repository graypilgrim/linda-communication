#ifndef STRING_OR_NUMBER_HPP
#define STRING_OR_NUMBER_HPP

#include <string>

class StringOrNumber
{
public:
	enum class Type
	{
		empty,
		string,
		number
	};

	StringOrNumber();
	StringOrNumber(const char *s);
	StringOrNumber(int n);

	Type getType();
	std::string getString();
	int getNumber();

private:
	std::string stringValue;
	int numberValue;
	Type type;
};

#endif
