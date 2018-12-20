#pragma once
#include "Synchronization.h"
#include "Semaphore.h"

class Mutex : public Synchronization {
public:
	Mutex(int = 0);
	status_t Lock();
	void Unlock();
private:
	virtual void ThreadWoken();
};

/// Reader/Writer lock
class RWLock {
public:
	RWLock();
	status_t LockRead();
	void UnlockRead();
	status_t LockWrite();
	void UnlockWrite();

private:
	volatile int fCount;
	Semaphore fWriteSem;
	Semaphore fReadSem;
	Mutex fWriteLock;
};


class RecursiveLock {
public:
	RecursiveLock(const char name[]);
	status_t Lock();
	void Unlock();
	bool IsLocked() const;
private:
	Mutex fMutex;
	class Thread *fHolder;
	int fRecursion;
};
