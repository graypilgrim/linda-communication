#ifndef TUPLE_HPP
#define TUPLE_HPP

#include "StringOrNumber.hpp"

#include <vector>
#include <memory>

class Tuple
{
public:
	Tuple() = default;
	Tuple(std::vector<StringOrNumber> values);

	void print();
	std::unique_ptr<unsigned char> rawFormat();
	void append(StringOrNumber value);

private:
	std::vector<StringOrNumber> values;
};

#endif
