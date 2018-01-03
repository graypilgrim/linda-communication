#ifndef UTILS_HPP
#define UTILS_HPP

#include <semaphore.h>

constexpr int ELEM_SIZE = 256;

// header + mutex and ref counter
constexpr int ELEM_HEADER_SIZE = 4 * sizeof(int) + sizeof(sem_t);

// the integer is a pointer to the next Elem
constexpr int TUPLE_SIZE = ELEM_SIZE - ELEM_HEADER_SIZE;

constexpr int MAX_TUPLES_COUNT = 4096;

constexpr int SHM_HEADER_SIZE = 4 * sizeof(int) + 2 * sizeof(sem_t);

constexpr int SHM_SIZE = ELEM_SIZE * MAX_TUPLES_COUNT + SHM_HEADER_SIZE;


enum class FieldType
{
	string,
	number,
	none
};

#endif
