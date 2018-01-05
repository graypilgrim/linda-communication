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

Tuple::Tuple(unsigned char* rawTuple)
{
	auto size = *(reinterpret_cast<unsigned int *>(rawTuple));

	auto descriptors = reinterpret_cast<TupleElemDescriptor *>(rawTuple + sizeof(unsigned int));
	for (auto i = 0u; i < size; ++i) {
		if (descriptors[i].type == STRING_T) {
			auto s = reinterpret_cast<char *>(rawTuple + descriptors[i].offset);
			auto val = StringOrNumber{s};
			values.emplace_back(val);
		} else {
			auto n = reinterpret_cast<int*>(rawTuple + descriptors[i].offset);
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

std::unique_ptr<unsigned char> Tuple::rawFormat()
{
	auto rawTuple = new unsigned char[TUPLE_SIZE];

	auto size = reinterpret_cast<unsigned int *>(rawTuple);
	*size = values.size();

	auto descriptors = reinterpret_cast<TupleElemDescriptor *>(rawTuple + sizeof(unsigned int));
	unsigned int offset = sizeof(unsigned int) + *size * sizeof(TupleElemDescriptor);
	for (auto i = 0u; i < values.size(); ++i) {
		descriptors[i].offset = offset;
		if (values[i].getType() == FieldType::string) {
			descriptors[i].type = STRING_T;
			memcpy(rawTuple + offset, values[i].getString().c_str(), values[i].getString().size() + 1);
			offset += values[i].getString().size() + 1;
		} else {
			descriptors[i].type = NUMBER_T;
			auto val = reinterpret_cast<int*>(rawTuple + offset);
			*val = values[i].getNumber();
			offset += sizeof(int);
		}
	}

	return std::unique_ptr<unsigned char>(rawTuple);
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
