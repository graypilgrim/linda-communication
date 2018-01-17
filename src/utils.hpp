#ifndef UTILS_HPP
#define UTILS_HPP

#include <semaphore.h>

constexpr int ELEM_SIZE = 256;

constexpr int ELEM_SYNC_SIZE = sizeof(int) + sizeof(sem_t);
constexpr int ELEM_HEADER_SIZE = 3 * sizeof(int) + ELEM_SYNC_SIZE;

// the integer is a pointer to the next Elem
constexpr int TUPLE_SIZE = ELEM_SIZE - ELEM_HEADER_SIZE;

constexpr int MAX_TUPLES_COUNT = 4096;
//constexpr int MAX_TUPLES_COUNT = 8;

constexpr int CONDITION_SIZE = sizeof(int) + 2 * sizeof(sem_t);
constexpr int SHM_HEADER_SIZE = 2 * sizeof(int) + 2 * sizeof(sem_t) + CONDITION_SIZE;
constexpr int SHM_SIZE = ELEM_SIZE * MAX_TUPLES_COUNT + SHM_HEADER_SIZE;


enum class FieldType
{
	string,
	number,
	none
};



#endif
