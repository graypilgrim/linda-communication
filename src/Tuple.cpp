#include "Tuple.hpp"

#include <iostream>

Tuple::Tuple(std::vector<StringOrNumber> values)
{
	this->values = std::move(values);
}

void Tuple::print()
{
	std::cout << "(";
	for (auto i = 0u; i < values.size(); ++i) {
		if (values[i].getType() == StringOrNumber::Type::number)
			std::cout << values[i].getNumber();

		if (values[i].getType() == StringOrNumber::Type::string)
			std::cout << "\"" << values[i].getString() << "\"";

		if (i + 1 < values.size())
			std::cout << ", ";
	}
	std::cout << ")" << std::endl;
}

std::unique_ptr<unsigned char> Tuple::rawFormat()
{

}

void Tuple::append(StringOrNumber value)
{
	values.emplace_back(std::move(value));
}
