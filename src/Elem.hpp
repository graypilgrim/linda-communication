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
	void* nextElemPtr()const;
	void* prevElemPtr()const;
	void* getAddr(int index)const {
		if (index != static_cast<int>(Index::Tail)
				&& index != static_cast<int>(Index::Invalid))
			return (char*)shmPtr + SHM_HEADER_SIZE + ELEM_SIZE * index;
		else
			return nullptr;
	}

	// void incRef() {
	//	   sync.incRef();
	// }
	// void decRef() {
	//	   sync.decRef();
	// }

};

#endif
