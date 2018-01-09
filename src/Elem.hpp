#ifndef ELEM_HPP
#define ELEM_HPP

#include "sync.hpp"
#include "Tuple.hpp"
#include "QueryParser.hpp"

// special index values for Elem
enum class Index : int {
	End = -1,
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
	Elem(char* shmPtr, int index, bool initialized=true);

	// TODO: consider not deleted copy constructor for iterator-like convention
	// I observed that it is not necessary, so I made it deleted.
	Elem(const Elem&) = delete;
	// Elem& operator=(const Elem&) = delete;

	~Elem();

	void init();
	void free();

	void lock() { sync.getMutex().lock(); }
	void unlock() { sync.getMutex().unlock(); }

	int getIndex()const { return index; }
	int getNextIndex()const { return header->nextElemIndex; }

	// move to the next block (increment operator)
	void next();

	std::optional<Tuple> read(const QueryVec&);
	std::optional<Tuple> take(const QueryVec&);

	// Elem should be locked before execution
	void setNextIndex(const int&);
	void setPrevIndex(const int&);

	char* getTupleBodyPtr()const;

private:

	// pointer to the global shared memory
	char* shmPtr;

	int index;

	// address of the block in the shared memory
	char* addr;

	ElemHeader* header;
	ElemSync sync;

	/*
	 * Returns address in memory of block by index.
	 * If index equals Index::Tail or Index::Invalid, returns null.
	 */
	char* getAddr(int index)const;

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
