#ifndef ELEM_HPP
#define ELEM_HPP

#include "sync.hpp"
#include "Tuple.hpp"
#include "QueryParser.hpp"

// special index values for Elem
enum class Index : int {
	REnd = -1,
	End = -2,
	Invalid = -3,
};

struct ElemHeader {
	int status;
	int prevElemIndex;
	int nextElemIndex;
};


class Elem {
public:

	enum class Status : uint8_t{
		Free,
		Valid,
		Zombie,
	};

	Elem(char* shmPtr, int index, bool hasRef=true);
	Elem(Elem&&);
	Elem(const Elem&)=delete;
	Elem& operator=(Elem&&);
	Elem& operator=(const Elem&)=delete;

	~Elem();

	void init();
	void free();

	ElemSync& getSync() { return sync; }
	void lock() { sync.getMutex().lock(); }
	void unlock() { sync.getMutex().unlock(); }

	int getIndex()const { return index; }
	int getNextIndex()const { return header->nextElemIndex; }
	int getPrevIndex()const { return header->prevElemIndex; }

	// move to the next block (increment operator)
	bool next(double& timeout);

	std::optional<Tuple> read(const QueryVec&);
	std::optional<Tuple> take(const QueryVec&);

	// Elem should be locked before execution
	void setStatus(const Status&);
	void setNextIndex(const int&);
	void setPrevIndex(const int&);

	char* getTupleBodyPtr()const;
	Status getStatus()const;
	char* getAddr()const { return addr; }

	void print()const;

private:

	void assign(Elem&& e);

	// if should increase/decrease reference
	bool hasRef;

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
	 * try deleting it (may be unsuccessful because of synchronization).
	 * Current index becomes the next element.
	 */
	bool tryDelete();

	/*
	 * Called at the beginning of some methods for avoiding bugs.
	 */
	void assertValid()const;

	std::optional<Tuple> readTakeImpl(const QueryVec& query, bool take);

};

#endif
