#include "Elem.hpp"
#include "Buffer.hpp"
#include "utils.hpp"

#include <cassert>


Elem::Elem(char* shmPtr, int index, bool hasRef):
		hasRef(hasRef),
		shmPtr(shmPtr),
		index(index),
		addr(getAddr(index)) {
	if (addr != nullptr) {
		// reinterpret memory fragment under addr
		header = reinterpret_cast<ElemHeader*>((char*)addr);
		sync = ElemSync((char*)addr + sizeof(ElemHeader));
		if (hasRef) {
			sync.incRef();
		}
	}
}

Elem::Elem(Elem&& e) {
	*this = std::move(e);
}

Elem& Elem::operator=(Elem&& e) {
	hasRef = e.hasRef;
	shmPtr = e.shmPtr;
	index = e.index;
	addr = e.addr;
	header = e.header;
	sync = e.sync;

	// only the new elem is responsible should sync.decRef() in destuctor
	e.hasRef = false;
	return *this;
}

Elem::~Elem() {
	if ((addr != nullptr) && hasRef) {
		sync.decRef();
	}
}

void Elem::init() {
	header -> status = static_cast<int>(Status::Free);
	sync.init();
}

void Elem::free() {
	sync.free();
}

bool Elem::next(double& timeout) {
	assertValid();
	tryDelete();

	ShmHeader shmHeader(shmPtr);

	auto guard = sync.getMutex().guardLock();

	/*
	 * Following loop is not an active waiting.
	 */
	while (header->nextElemIndex == static_cast<int>(Index::End)) {
		auto condGuard = shmHeader.cond.mutex.guardLock();

		if (header->nextElemIndex != static_cast<int>(Index::End)) {
			*this = Elem(shmPtr, header->nextElemIndex);
			return true;
		}

		unlock();
		std::cerr << "waiting for new" << std::endl;
		if (!shmHeader.cond.wait(timeout)) {
			std::cerr << "waiting timeout" << std::endl;
			lock();
			return false;
		} else {
			std::cerr << "waiting end" << std::endl;
		}
		lock();
	}

	*this = Elem(shmPtr, header->nextElemIndex);
	return true;
}

std::optional<Tuple> Elem::read(const QueryVec& query) {
	return readTakeImpl(query, false);
}

std::optional<Tuple> Elem::take(const QueryVec& query) {
	return readTakeImpl(query, true);
}

void Elem::setStatus(const Status& i) {
	assertValid();
	header->status = static_cast<int>(i);
}

void Elem::setNextIndex(const int& i) {
	assertValid();
	header->nextElemIndex = i;
}

void Elem::setPrevIndex(const int& i) {
	assertValid();
	header->prevElemIndex = i;
}

char* Elem::getTupleBodyPtr()const {
	assertValid();
	return (char*)addr + sizeof(ElemHeader) + sizeof(sem_t) + sizeof(int);
}

Elem::Status Elem::getStatus()const {
	assertValid();
	return static_cast<Status>(header->status);
}

void Elem::print()const {
	std::cout << "Elem at: " << (long long int)addr << ", index: " << getIndex() << std::endl;

	std::cout << "\tStatus: ";
	if (getStatus() == Elem::Status::Free){
		std::cout << "Free" << std::endl;
		std::cout << "\tTuple: None" << std::endl;
	} else if (getStatus() == Elem::Status::Zombie) {
		std::cout << "Zombie" << std::endl;
		std::cout << "\tTuple: ";
		Tuple t((unsigned char*)getTupleBodyPtr());
		t.print();
	}else {
		assert(getStatus() == Status::Valid);
		std::cout << "Valid" << std::endl;
		std::cout << "\tTuple: ";
		Tuple t((unsigned char*)getTupleBodyPtr());
		t.print();
	}

	std::cout << "\tReferences: " << sync.getRefCount() << std::endl;

	std::cout << "\tPrevIndex: " << header->prevElemIndex << std::endl;
	std::cout << "\tNextIndex: " << header->nextElemIndex << std::endl;
}

char* Elem::getAddr(int index) const {
	if (index != static_cast<int>(Index::End)
			&& index != static_cast<int>(Index::Invalid))
		return (char*)shmPtr + SHM_HEADER_SIZE + ELEM_SIZE * index;
	else
		return nullptr;
}

void Elem::tryDelete() {
	assertValid();
	// TODO: implement
	// remember that 3 locks are required to delete the element
	// if (header->status == static_cast<int>(Status::Zombie)) {
	// 	if (sync.getRefCount() == 1) {
	// 	}
	// }
}

void Elem::assertValid()const {
	assert(addr != nullptr);
	assert(getIndex() != static_cast<int>(Index::End));
	assert(getIndex() != static_cast<int>(Index::Invalid));
}


std::optional<Tuple> Elem::readTakeImpl(const QueryVec& query, bool take) {
	assertValid();
	assert(getStatus() != Status::Free);
	if (header->status == static_cast<int>(Status::Zombie)) {
		return std::nullopt;
	}

	Tuple tuple((unsigned char*)getTupleBodyPtr());
	if (!tuple.match(query)) {
		return std::nullopt;
	}

	if (take) {
		auto guard = sync.getMutex().guardLock();
		if (header->status == static_cast<int>(Status::Zombie)) {
			// this tuple haas been taken in the middletime
			return std::nullopt;
		}
		assert(header->status == static_cast<int>(Status::Valid));
		header->status = static_cast<int>(Status::Zombie);
	}

	return tuple;
}
