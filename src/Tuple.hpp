#ifndef TUPLE_HPP
#define TUPLE_HPP

#include "StringOrNumber.hpp"
#include "utils.hpp"
#include "QueryParser.hpp"

#include <vector>
#include <memory>

class Tuple
{
public:
	Tuple() = default;
	Tuple(std::vector<StringOrNumber> values);
	Tuple(unsigned char* addr);

	void print();

	/*
	 * Dump into memory, so that the constructor
	 * can be layer called with the same argument.
	 */
	void write(unsigned char* addr)const;

	void append(StringOrNumber value);
	bool match(const QueryVec &queries);
	bool empty();

private:
	std::vector<StringOrNumber> values;

	static constexpr unsigned char STRING_T = 0x0F;
	static constexpr unsigned char NUMBER_T = 0xF0;
};

#endif
