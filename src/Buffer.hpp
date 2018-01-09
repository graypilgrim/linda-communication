#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "Tuple.hpp"
#include "Elem.hpp"

#include <optional>
#include <string>


// TODO: consider moving other file
struct ShmHeader {
	ShmHeader(void* shmPtr):
			headLock(shmPtr),
			headIndex(reinterpret_cast<int*>((char*)shmPtr + sizeof(sem_t))),
			tailLock((char*)shmPtr + sizeof(sem_t) + sizeof(int)),
			tailIndex(reinterpret_cast<int*>((char*)shmPtr + sizeof(int) + 2 * sizeof(sem_t))) {
	}
	Mutex headLock;
	int* headIndex;
	Mutex tailLock;
	int* tailIndex;
};


class Buffer
{
public:
	enum class OutputResult {
		success,
		out_of_memory
	};

	Buffer(const std::string &shmName, bool initialized=true);

	/*
	 * Create shared memory, create all semaphores
	 * and init all blocks (set Elem::Free status).
	 */
	void init();

	/*
	 * Free all resources.
	 */
	void destroy();

	OutputResult output(const Tuple &tuple);
	std::optional<Tuple> input(const std::string &query, unsigned int timeout);
	std::optional<Tuple> read(const std::string &query, unsigned int timeout);

private:
	std::string shmName;
	int shmFd;
	char* shmPtr;

	Elem getFirstElem();
	Elem getLastElem();

	/*
	 * Returns Elem with status Valid
	 * or with index Invalid if there is no free block.
	 * Returned Elem is locked.
	 */
	Elem findFreeBlock();

	// Implementation of input and read methods.
	std::optional<Tuple> inputReadImpl(const std::string &query,
			unsigned int timeout, bool deleteTuple);
};

#endif
