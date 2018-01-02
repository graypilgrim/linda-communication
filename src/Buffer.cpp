#include "Buffer.hpp"

Buffer::Buffer()
{
}

Buffer::OutputResult Buffer::output(const Tuple &tuple)
{
	return {};
}

std::optional<Tuple> Buffer::input(const std::string &query, unsigned int timeout)
{
	return {};
}

std::optional<Tuple> Buffer::read(const std::string &query, unsigned int timeout)
{
	return {};
}
