#include "Buffer.hpp"
#include "Elem.hpp"
#include "QueryLexer.hpp"
#include "QueryParser.hpp"

#include <sys/mman.h>
// #include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

Buffer::Buffer(const std::string &shmName)
{
	// TODO: sever should create shm and all semaphores
	shmFd = shm_open(shmName.c_str(), O_CREAT, O_RDWR);
	ftruncate(shmFd, SHM_SIZE);
	shmPtr = mmap(nullptr, SHM_SIZE, PROT_WRITE, MAP_SHARED, shmFd, 0);
}

Buffer::OutputResult Buffer::output(const Tuple &tuple)
{
	Elem freeBlock = findFreeBlock();
	if (freeBlock.getIndex() == static_cast<int>(Index::Invalid))
		return OutputResult::out_of_memory;

	ShmHeader shmHeader(shmPtr);

	/* Acquire 2 locks: last element and tail pointer.
		* This is not an active waiting.
		* When adding first element, we need head lock instead of last element lock,
		* because it doesn't exist.
		* */
	bool addingFirstElement;
	Elem last = getLastElem();
	while (true) {
		addingFirstElement = false;
		if (last.getIndex() == static_cast<int>(Index::Tail)) {
			// adding first element
			shmHeader.headLock.lock();

			// check if someone didn't add first element in the midtime.
			if (*shmHeader.headIndex == static_cast<int>(Index::Tail)) {
				addingFirstElement = true;
				break;
			}

			// someone added element in midtime
			last = getLastElem();
			continue;
		}
		last.lock();
		if (last.getNextIndex() == static_cast<int>(Index::Tail))
			break;

		// someone added element in midtime
		last.unlock();
		last = getLastElem();
		continue;
	}
	shmHeader.tailLock.lock();

	// safely add element to list (create connections)
	if (addingFirstElement) {
		*shmHeader.headIndex = freeBlock.getIndex();
		freeBlock.setPrevIndex(*shmHeader.headIndex);
	} else {
		last.setNextIndex(freeBlock.getIndex());
		freeBlock.setPrevIndex(last.getIndex());
	}
	// TODO: tuple.writeToAddress(freeBlock->getTupleBodyPtr());
	*shmHeader.tailIndex = freeBlock.getIndex();

	// unlock everything
	if (addingFirstElement) {
		shmHeader.headLock.unlock();
	} else {
		last.unlock();
	}
	shmHeader.tailLock.unlock();
	freeBlock.unlock();

	return OutputResult::success;
}

std::optional<Tuple> Buffer::input(const std::string &query, unsigned int timeout) {
	return inputReadImpl(query, timeout, true);
}

std::optional<Tuple> Buffer::read(const std::string &query, unsigned int timeout) {
	return inputReadImpl(query, timeout, false);
}

Elem Buffer::getFirstElem()
{
	ShmHeader shmHeader(shmPtr);
	auto g = shmHeader.headLock.guardLock();
	return Elem(shmPtr, *shmHeader.headIndex);
}

Elem Buffer::getLastElem()
{
	ShmHeader shmHeader(shmPtr);
	auto g = shmHeader.tailLock.guardLock();
	return Elem(shmPtr, *shmHeader.tailIndex);
}

Elem Buffer::findFreeBlock() {
	// TODO: implement remembering following 2 things:
	// freeBlock.getSync().lock();
	// freeBlock.setNextIndex(Index::Tail);
	return Elem(shmPtr, static_cast<int>(Index::Invalid));
}

std::optional<Tuple> Buffer::inputReadImpl(const std::string &query,
		unsigned int timeout, bool deleteTuple) {
	QueryLexer ql{query};
	auto tokens = ql.tokenize();
	QueryParser qp{tokens};
	auto queries = qp.parse();

	// TODO: implement timeout
	// consider adding argument to the Elem::next() method.
	Elem cur = getFirstElem();
	while (true) {
		if (deleteTuple) {
			if (auto result = cur.take(queries))
				return result.value();
		} else {
			if (auto result = cur.read(queries))
				return result.value();
		}
		cur.next();
	}
	return std::nullopt;
}
