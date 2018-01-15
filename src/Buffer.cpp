#include "Buffer.hpp"
#include "Elem.hpp"
#include "QueryLexer.hpp"
#include "QueryParser.hpp"

#include <sys/mman.h>
// #include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>

Buffer::Buffer(const std::string &shmName, bool initialized):
		shmName(shmName),
		currentAllocationIndex(0) {
	if (initialized) {
		// TODO: check what these flags do in this case (shm object exists)
		// auto mode = S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IWOTH | S_IROTH;

		// TODO: error handling
		shmFd = shm_open(shmName.c_str(), O_RDWR, 0);
		std::cerr << "shm_open: " << strerror(errno) << std::endl;
		if (shmFd < 0)
			exit(errno);

		shmPtr = (char*)mmap(nullptr, SHM_SIZE, PROT_WRITE, MAP_SHARED, shmFd, 0);
		std::cerr << "mmap: " << strerror(errno) << std::endl;
	}
}

void Buffer::init() {
	// TODO: error handling

	auto mode = S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IWOTH | S_IROTH;
	shmFd = shm_open(shmName.c_str(), O_CREAT | O_RDWR, mode);
	std::cerr << "shm_open: " << strerror(errno) << std::endl;
	auto result = ftruncate(shmFd, SHM_SIZE);
	std::cerr << "ftruncate: " << strerror(errno) << std::endl;
	if (result < 0)
		exit(errno);

	shmPtr = (char*)mmap(nullptr, SHM_SIZE, PROT_WRITE, MAP_SHARED, shmFd, 0);
	std::cerr << "mmap: " << strerror(errno) << std::endl;

	ShmHeader shmHeader(shmPtr);
	shmHeader.headLock.init();
	*shmHeader.headIndex = static_cast<int>(Index::End);
	shmHeader.tailLock.init();
	*shmHeader.tailIndex = static_cast<int>(Index::End);
	shmHeader.cond.init();

	for (int i = 0; i < MAX_TUPLES_COUNT ; ++i) {
		Elem block(shmPtr, i, false);
		block.init();
	}
}

void Buffer::destroy() {
	for (int i = 0; i < MAX_TUPLES_COUNT ; ++i) {
		Elem block(shmPtr, i, false);
		block.free();
	}

	ShmHeader shmHeader(shmPtr);
	shmHeader.headLock.free();
	std::cerr << "headLock.free(): " << strerror(errno) << std::endl;
	shmHeader.tailLock.free();
	std::cerr << "tailLock.free(): " << strerror(errno) << std::endl;
	shmHeader.cond.free();
	std::cerr << "cond.free(): " << strerror(errno) << std::endl;

	munmap(shmPtr, SHM_SIZE);
	std::cerr << "munmap: " << strerror(errno) << std::endl;
	close(shmFd);
	shm_unlink(shmName.c_str());
	std::cerr << "shm_unlink: " << strerror(errno) << std::endl;
}

Buffer::OutputResult Buffer::output(const Tuple &tuple)
{
	Elem freeBlock(findFreeBlock());
	if (freeBlock.getIndex() == static_cast<int>(Index::Invalid))
		return OutputResult::out_of_memory;

	ShmHeader shmHeader(shmPtr);

	/* Acquire 2 locks: last element and tail pointer.
		* Following loop is not an active waiting.
		* When adding first element, we need head lock instead of last element lock,
		* because it doesn't exist.
		* */
	bool addingFirstElement;
	Elem last = getLastElem();
	while (true) {
		addingFirstElement = false;
		if (last.getIndex() == static_cast<int>(Index::End)) {
			// adding first element
			shmHeader.headLock.lock();

			// check if someone didn't add first element in the midtime.
			if (*shmHeader.headIndex == static_cast<int>(Index::End)) {
				addingFirstElement = true;
				break;
			}

			// someone added element in midtime
			last = getLastElem();
			continue;
		}
		last.lock();
		if (last.getNextIndex() == static_cast<int>(Index::End))
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
		freeBlock.setPrevIndex(static_cast<int>(Index::REnd));
	} else {
		last.setNextIndex(freeBlock.getIndex());
		freeBlock.setPrevIndex(last.getIndex());
	}
	tuple.write((unsigned char*)freeBlock.getTupleBodyPtr());
	freeBlock.setStatus(Elem::Status::Valid);
	*shmHeader.tailIndex = freeBlock.getIndex();

	// unlock everything
	if (addingFirstElement) {
		shmHeader.headLock.unlock();
	} else {
		last.unlock();
	}
	shmHeader.tailLock.unlock();
	freeBlock.unlock();

	shmHeader.cond.mutex.lock();
	std::cerr << "broadcast" << std::endl;
	shmHeader.cond.broadcast();
	shmHeader.cond.mutex.unlock();

	return OutputResult::success;
}

std::optional<Tuple> Buffer::input(const std::string &query, double timeout) {
	return inputReadImpl(query, timeout, true);
}

std::optional<Tuple> Buffer::read(const std::string &query, double timeout) {
	return inputReadImpl(query, timeout, false);
}

void Buffer::print()const {
	std::cout << "--------------------------" << std::endl;
	std::cout << "Total number of blocks: " << MAX_TUPLES_COUNT << std::endl;
	for (int i = 0; i < MAX_TUPLES_COUNT ; ++i) {
		Elem block(shmPtr, i);
		block.print();
	}
}

void Buffer::printList()const {
	std::cout << "--------------------------" << std::endl;
	Elem e = getFirstElem();
	if (e.getIndex() == static_cast<int>(Index::End)) {
		std::cout << "<empty>" << std::endl;
		return;
	}

	double timeout = 0.5;
	do {
		e.print();
	} while(e.next(timeout));
}

Elem Buffer::getFirstElem()const
{
	ShmHeader shmHeader(shmPtr);
	auto g = shmHeader.headLock.guardLock();
	return Elem(shmPtr, *shmHeader.headIndex);
}

Elem Buffer::getLastElem()const
{
	ShmHeader shmHeader(shmPtr);
	auto g = shmHeader.tailLock.guardLock();
	return Elem(shmPtr, *shmHeader.tailIndex);
}

Elem Buffer::findFreeBlock() {
	// TODO: implement some break condition(s) like timeout
	while (true) {
		Elem block(shmPtr, currentAllocationIndex);
		block.lock();
		if (block.getStatus() == Elem::Status::Free) {
			block.setNextIndex(static_cast<int>(Index::End));
			return std::move(block);
		}
		block.unlock();
		currentAllocationIndex = (currentAllocationIndex + 1) % MAX_TUPLES_COUNT;
	}
}

std::optional<Tuple> Buffer::inputReadImpl(const std::string &query,
		double& timeout, bool deleteTuple) {
	QueryLexer ql{query};
	auto tokens = ql.tokenize();
	QueryParser qp{tokens};
	auto queries = qp.parse();

	ShmHeader shmHeader(shmPtr);

	// TODO: consider including other short operations times in timeout
	Elem cur = getFirstElem();

	while (cur.getIndex() == static_cast<int>(Index::End)) {
		auto shmHeaderGuard = shmHeader.cond.mutex.guardLock();
		if (cur.getIndex() == static_cast<int>(Index::End)) {
			std::cerr << "waiting for new" << std::endl;
			if (!shmHeader.cond.wait(timeout)) {
				std::cerr << "waiting timeout" << std::endl;
				return std::nullopt;
			} else {
				std::cerr << "waiting end" << std::endl;
			}
		}
		cur = getFirstElem();
	}

	while (true) {
		if (deleteTuple) {
			if (auto result = cur.take(queries))
				return result.value();
		} else {
			if (auto result = cur.read(queries))
				return result.value();
		}
		if (!cur.next(timeout))
			return std::nullopt;
	}
	return std::nullopt;
}
