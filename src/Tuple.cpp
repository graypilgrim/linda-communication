#include "Tuple.hpp"
#include "utils.hpp"

#include <iostream>
#include <cstring>

struct TupleElemDescriptor
{
	unsigned char type;
	unsigned int offset;
};

Tuple::Tuple(std::vector<StringOrNumber> values)
{
	this->values = std::move(values);
}

Tuple::Tuple(unsigned char* addr)
{
	auto size = *(reinterpret_cast<unsigned int *>(addr));

	auto descriptors = reinterpret_cast<TupleElemDescriptor *>(addr + sizeof(unsigned int));
	for (auto i = 0u; i < size; ++i) {
		if (descriptors[i].type == STRING_T) {
			auto s = reinterpret_cast<char *>(addr + descriptors[i].offset);
			auto val = StringOrNumber{s};
			values.emplace_back(val);
		} else {
			auto n = reinterpret_cast<int*>(addr + descriptors[i].offset);
			auto val = StringOrNumber{*n};
			values.emplace_back(val);
		}
	}
}

void Tuple::print()
{
	std::cout << "(";
	for (auto i = 0u; i < values.size(); ++i) {
		if (values[i].getType() == FieldType::number)
			std::cout << values[i].getNumber();

		if (values[i].getType() == FieldType::string)
			std::cout << "\"" << values[i].getString() << "\"";

		if (i + 1 < values.size())
			std::cout << ", ";
	}
	std::cout << ")" << std::endl;
}


void Tuple::write(unsigned char* addr)const {
	auto size = reinterpret_cast<unsigned int *>(addr);
	*size = values.size();

	auto descriptors = reinterpret_cast<TupleElemDescriptor *>(addr + sizeof(unsigned int));
	unsigned int offset = sizeof(unsigned int) + *size * sizeof(TupleElemDescriptor);
	for (auto i = 0u; i < values.size(); ++i) {
		descriptors[i].offset = offset;
		if (values[i].getType() == FieldType::string) {
			descriptors[i].type = STRING_T;
			memcpy(addr + offset, values[i].getString().c_str(), values[i].getString().size() + 1);
			offset += values[i].getString().size() + 1;
		} else {
			descriptors[i].type = NUMBER_T;
			auto val = reinterpret_cast<int*>(addr + offset);
			*val = values[i].getNumber();
			offset += sizeof(int);
		}
	}
}

void Tuple::append(StringOrNumber value)
{
	values.emplace_back(std::move(value));
}

bool Tuple::match(const QueryVec& queries) {
	if (queries.size() != values.size())
		return false;

	// TODO: consider using boost::combine
	for (auto [query, value] = make_pair(queries.begin(), values.begin()); query!=queries.end() ; ++query, ++value) {
		if (!query->second(*value))
			return false;
	}
	return true;
}
