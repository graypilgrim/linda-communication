#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "Tuple.hpp"

#include <optional>
#include <string>

class Buffer
{
public:
	enum class OutputResult {
		success,
		out_of_memory
	};

	Buffer();

	OutputResult output(const Tuple &tuple);
	std::optional<Tuple> input(const std::string &query, unsigned int timeout);
	std::optional<Tuple> read(const std::string &query, unsigned int timeout);
};

#endif
