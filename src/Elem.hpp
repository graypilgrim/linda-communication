#ifndef ELEM_HPP
#define ELEM_HPP

#include "sync.hpp"
#include "QueryParser.hpp"
#include "Tuple.hpp"

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
	// I observed that they are not necessary, so I made it deleted.
	Elem(const Elem&) = delete;
	// Elem& operator=(const Elem&) = delete;

	~Elem();

	void lock() {
		sync.getMutex().lock();
	}
	void unlock() {
		sync.getMutex().unlock();
	}

	int getIndex()const {
		return index;
	}

	// move to the next block
	void next();

	std::optional<Tuple> read(const QueryParser::Query&);
	std::optional<Tuple> take(const QueryParser::Query&);

	// Elem should be locked before execution
	void setNextElem(const Elem&);
	void setPrevElem(const Elem&);

private:
	void* shmPtr;
	int index;
	void* addr;

	ElemHeader* header;
	ElemSync sync;

	void* getTupleBodyPtr()const;
	void* getNextElemPtr()const;
	void* getPrevElemPtr()const;
	void* getAddr(int index)const;

};

#endif
