#include "Elem.hpp"
#include "utils.hpp"

#include <cassert>


Elem::Elem(void* shmPtr, int index):
		shmPtr(shmPtr),
		index(index),
		addr(getAddr(index)) {
	if (addr != nullptr) {
		// reinterpret memory fragment under addr
		header = reinterpret_cast<ElemHeader*>((char*)addr);
		sync = ElemSync((char*)addr + sizeof(ElemHeader));
		sync.incRef();
	}
}

Elem::~Elem() {
	if (addr != nullptr) {
		sync.decRef();
	}
}

void Elem::next() {
	auto guard = sync.getMutex().guardLock();
	if (header->nextElemIndex == static_cast<int>(Index::Tail)) {
		// TODO: implement waiting for new elements here
		assert(0);
	}
	*this = Elem(shmPtr, header->nextElemIndex);
}

std::optional<Tuple> Elem::read(const QueryVec& tuple) {
	// TODO: implement sometching like this:
	// tuple = Tuple::fromAddr(tupleBodyPtr)
	// if (query.match(tuple)) {
	//	   return tuple;
	// }
	return std::nullopt;
}

std::optional<Tuple> Elem::take(const QueryVec& tuple) {
	// TODO: implement sometching similar to read.
	// The only difference should be that this method changes status to Zombie.

	lock();
	assert(header->status != static_cast<int>(Status::Free));
	if (header->status == static_cast<int>(Status::Zombie))
		return std::nullopt;
	// tuple = Tuple::fromAddr(tupleBodyPtr)
	// if (query.match(tuple)) {
	//	   return tuple;
	// }
	unlock();
	return std::nullopt;
}

void Elem::setNextElem(const Elem& e) {
	header->nextElemIndex = e.getIndex();
}

void Elem::setPrevElem(const Elem& e) {
	header->prevElemIndex = e.getIndex();
}

void* Elem::getTupleBodyPtr()const {
	return (char*)addr + sizeof(ElemHeader) + sizeof(sem_t) + sizeof(int);
}

void* Elem::getNextElemPtr()const {
	return getAddr(header->nextElemIndex);
}

void* Elem::getPrevElemPtr()const {
	return getAddr(header->prevElemIndex);
}

void *Elem::getAddr(int index) const {
	if (index != static_cast<int>(Index::Tail)
			&& index != static_cast<int>(Index::Invalid))
		return (char*)shmPtr + SHM_HEADER_SIZE + ELEM_SIZE * index;
	else
		return nullptr;
}
