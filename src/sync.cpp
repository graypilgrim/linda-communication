#include "sync.hpp"

bool Mutex::lock(int timeout) {
	if (timeout == -1) {
		sem_wait(sem);
		return true;
	} else {
		return timedWait(timeout);
	}
}

bool Mutex::timedWait(int timeout) {
	if (timeout == -1)
		return sem_wait(sem);
	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
		// TODO: consider different handling
		std::cerr << "clock_gettime error";
		exit(EXIT_FAILURE);
	}
	ts.tv_sec += timeout;
	return sem_timedwait(sem, &ts) == 0;
}

void ElemSync::incRef() {
	auto g = mutex.guardLock();
	++refCounter;
}

void ElemSync::decRef() {
	auto g = mutex.guardLock();
	--refCounter;
}
