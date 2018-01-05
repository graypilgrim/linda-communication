#ifndef ELEM_HPP
#define ELEM_HPP

#include "sync.hpp"
#include "Tuple.hpp"
#include "QueryParser.hpp"

// special index values for Elem
enum class Index : int {
	Tail = -1,
	Invalid = -2
};

struct ElemHeader {
	int status;
	int nextElemIndex;
	int prevElemIndex;
};


class Elem {
	enum class Status : uint8_t{
		Free,
		Valid,
		Zombie,
	};

public:
	Elem(void* shmPtr, int index);

	// TODO: consider not deleted copy constructor for iterator-like convention
	// I observed that it is not necessary, so I made it deleted.
	Elem(const Elem&) = delete;
	// Elem& operator=(const Elem&) = delete;

	~Elem();

	void lock() { sync.getMutex().lock(); }
	void unlock() { sync.getMutex().unlock(); }

	int getIndex()const { return index; }
	int getNextIndex()const { return header->nextElemIndex; }

	// move to the next block
	void next();

	std::optional<Tuple> read(const QueryVec&);
	std::optional<Tuple> take(const QueryVec&);

	// Elem should be locked before execution
	void setNextIndex(const int&);
	void setPrevIndex(const int&);

private:

	// pointer to the global shared memory
	void* shmPtr;

	int index;

	// address of the block in the shared memory
	void* addr;

	ElemHeader* header;
	ElemSync sync;

	void* getTupleBodyPtr()const;

	/*
	 * Returns address in memory of block by index.
	 * If index equals Index::Tail or Index::Invalid, returns null.
	 */
	void* getAddr(int index)const;

	/*
	 * If current element has Zombie status
	 * and ref count is 1 (currently, only I need this element),
	 * try deleting it (may be unsuccessfull because synchronization results).
	 */
	void tryDelete();

	/*
	 * Called at the beginning of some methods for avoiding bugs.
	 */
	void assertValid()const;

};

#endif
