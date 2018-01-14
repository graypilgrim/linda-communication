#include "sync.hpp"

bool timedWait(sem_t* sem, int timeout){
	if (timeout == -1) {
		sem_wait(sem);
        return true;
    }
	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
		// TODO: consider different handling
		std::cerr << "clock_gettime error";
		exit(EXIT_FAILURE);
	}
	ts.tv_sec += timeout;
	return sem_timedwait(sem, &ts) == 0;
}

bool Mutex::lock(int timeout) {
	if (timeout == -1) {
		sem_wait(sem);
		return true;
	} else {
		return timedWait(sem, timeout);
	}
}


void ElemSync::incRef() {
	auto g = mutex.guardLock();
	++*refCounter;
}

void ElemSync::decRef() {
	auto g = mutex.guardLock();
	--*refCounter;
}

void ConditionVariable::wait(int timeout) {
    ++*waitersCount;

    mutex.unlock();

    // sem_wait(sem);
    if (!timedWait(sem, timeout)) {
        mutex.lock();
        --*waitersCount;
        mutex.unlock();
    }

    mutex.lock();
}

void ConditionVariable::broadcast(int timeout) {
    while (*waitersCount > 0) {
        --*waitersCount;
        sem_post(sem);
    }
}
