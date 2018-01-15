#include "sync.hpp"

bool timedWait(sem_t* sem, double& timeout){
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

    // TODO: decrease timeout here
    struct timespec start, finish;
    clock_gettime(CLOCK_REALTIME, &start);
	bool ret = (sem_timedwait(sem, &ts) == 0);
    clock_gettime(CLOCK_REALTIME, &finish);
    timeout -= (finish.tv_sec - start.tv_sec);
    return ret;
}

bool Mutex::lock() {
    sem_wait(sem);
    return true;
	// if (timeout == -1) {
	// 	return true;
	// } else {
	// 	return timedWait(sem, timeout);
	// }
}


void ElemSync::incRef(bool sync) {
	if (sync) {
		auto g = mutex.guardLock();
		++*refCounter;
	} else {
		++*refCounter;
	}
}

void ElemSync::decRef(bool sync) {
	if (sync) {
		auto g = mutex.guardLock();
		--*refCounter;
	} else {
		--*refCounter;
	}
}

bool ConditionVariable::wait(double& timeout) {
    ++*waitersCount;

    mutex.unlock();

    // sem_wait(sem);
    if (!timedWait(sem, timeout)) {
        mutex.lock();
        --*waitersCount;
        return false;
    }

    mutex.lock();
    return true;
}

void ConditionVariable::broadcast() {
    while (*waitersCount > 0) {
        --*waitersCount;
        sem_post(sem);
    }
}
