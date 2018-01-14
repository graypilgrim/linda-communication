#ifndef WRLOCK_HPP
#define WRLOCK_HPP

#include <semaphore.h>
#include <time.h>
#include <iostream>

#include <errno.h>
#include <string.h>

// TODO: consider moving some methods implementation to .cpp file
class Mutex {
	class LockGuard {
		Mutex& m;
		bool status;
	public:
		LockGuard(Mutex& m, int timeout=-1):m(m) {
			status = m.lock(timeout);
		}
		~LockGuard() {
			m.unlock();
		}

		bool getStatus() {
			return status;
		}
	};

public:
	Mutex(): sem(nullptr) {
	}
	Mutex(void* addr):
		sem(reinterpret_cast<sem_t*>(addr)) {
	}

	/* This method shouldn't be in constructor,
	 * because the semaphore sometimes already exist here.*/
	void init() {
		sem_init(sem, 1, 1);
	}

	/* This method shouldn't be in destructor.
	 * It should be called just before the shared memory is being unlinked. */
	void free() {
		sem_destroy(sem);
	}

	bool lock(int timeout=-1);

	void unlock() {
		sem_post(sem);
	}

	LockGuard guardLock(int timeout=-1) {
		return LockGuard(*this, timeout);
	}

private:
	sem_t* sem;

};

/*
 * A mutex and integer - ref counter.
 */
class ElemSync {
public:
	ElemSync():refCounter(nullptr) {
	}
	ElemSync(void* addr):
		mutex(addr),
		refCounter(reinterpret_cast<int*>((char*)addr + sizeof(sem_t))) {
	}

	void init() {
		mutex.init();
		*refCounter = 0;
	}

	void free() {
		mutex.free();
	}

	void incRef();

	void decRef();

	Mutex getMutex() {
		return mutex;
	}

	int getRefCount()const {
		return *refCounter;
	}

private:
	Mutex mutex;
	int* refCounter;

};


class ConditionVariable {
public:

    ConditionVariable(void* addr):
        mutex(addr),
        waitersCount(reinterpret_cast<int*>((char*) addr + sizeof(sem_t))),
        sem(reinterpret_cast<sem_t*>((char*) addr + sizeof(sem_t) + sizeof(int))) {
    }

    void init() {
        mutex.init();
		sem_init(sem, 1, 0);
        *waitersCount = 0;
    }

    void free() {
        mutex.free();
        sem_destroy(sem);
    }

    // precondition: mutex acquired
    void wait(int timeout=-1);

    // precondition: mutex acquired
    void broadcast(int timeout=-1);

    Mutex mutex;

private:
    int* waitersCount;
	sem_t* sem;

};


#endif
