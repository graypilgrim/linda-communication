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
	assertValid();
	tryDelete();

	auto guard = sync.getMutex().guardLock();
	if (header->nextElemIndex == static_cast<int>(Index::Tail)) {
		// TODO: implement waiting for new elements here
		// remember unlocking this element, so that anythin can be added.
		assert(0);
	}
	*this = Elem(shmPtr, header->nextElemIndex);
}

std::optional<Tuple> Elem::read(const QueryVec& query) {
	assertValid();
	// TODO: implement sometching like this:
	// tuple = Tuple::fromAddr(tupleBodyPtr)
	// if (tuple.match(query)) {
	//	   return tuple;
	// }
	return std::nullopt;
}

std::optional<Tuple> Elem::take(const QueryVec& query) {
	assertValid();
	lock();
	assert(header->status != static_cast<int>(Status::Free));
	if (header->status == static_cast<int>(Status::Zombie)) {
		// this tuple is already taken
		return std::nullopt;
	}
	// TODO: implement sometching similar to read.
	// The only difference should be that this method changes status to Zombie.

	unlock();
	tryDelete();
	return std::nullopt;
}

void Elem::setNextIndex(const int& i) {
	assertValid();
	header->nextElemIndex = i;
}

void Elem::setPrevIndex(const int& i) {
	assertValid();
	header->prevElemIndex = i;
}

void* Elem::getTupleBodyPtr()const {
	assertValid();
	return (char*)addr + sizeof(ElemHeader) + sizeof(sem_t) + sizeof(int);
}

void *Elem::getAddr(int index) const {
	if (index != static_cast<int>(Index::Tail)
			&& index != static_cast<int>(Index::Invalid))
		return (char*)shmPtr + SHM_HEADER_SIZE + ELEM_SIZE * index;
	else
		return nullptr;
}

void Elem::tryDelete() {
	assertValid();
	// TODO: implement
	// remember that 3 locks are required to delete the element
	if (header->status == static_cast<int>(Status::Zombie)) {
		if (sync.getRefCount() == 1) {
			// TODO
		}
	}
}

void Elem::assertValid()const {
	assert(addr != nullptr);
	assert(getIndex() != static_cast<int>(Index::Tail));
	assert(getIndex() != static_cast<int>(Index::Invalid));
}
