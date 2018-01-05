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
	auto last = getLastElem();
	Elem freeBlock = findFreeBlock();
	if (freeBlock.getIndex() != static_cast<int>(Index::Invalid)) {
		ShmHeader shmHeader(shmPtr);

		while (true) {
			last.lock();
			shmHeader.tailLock.lock();

			last.setNextElem(freeBlock);
			freeBlock.setPrevElem(last);
			*shmHeader.tailIndex = freeBlock.getIndex();
			// TODO:
			// tuple.writeToAddress(freeBlock->getTupleBodyPtr());

		}
		last.unlock();
		shmHeader.tailLock.unlock();
		freeBlock.unlock();
		return OutputResult::success;
	} else {
		return OutputResult::out_of_memory;
	}
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
	// TODO: implement remembering:
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
	Elem cur = getFirstElem();
	while (true) {
		if (deleteTuple) {
			if (auto result = cur.take(queries))
				return result.value();
		} else {
			if (auto result = cur.read(queries))
				return result.value();
		}
		// TODO: timeout may be passed as argument of the next method
		cur.next();
	}
	return std::nullopt;
}
