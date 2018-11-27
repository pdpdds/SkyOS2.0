#pragma once
#include "windef.h"
#include "types.h"
#include "Queue.h"

class Synchronization
{
public:
	Synchronization();
	~Synchronization();

	
	status_t Wait(bigtime_t timeout = INFINITE_TIMEOUT);
	static status_t WaitForMultipleSyncObject(int dispatcherCount, Synchronization *syncObjects[],
		WaitFlags flags, bigtime_t timeout = INFINITE_TIMEOUT);

protected:	
	void Signal(bool reschedule);
	void Unsignal();
	virtual void ThreadWoken();

private:
	static status_t WaitInternal(int dispatcherCount, Synchronization *syncObjects[],
		WaitFlags flags, bigtime_t timeout, class WaitTag[]);
	bool fSignalled;
	Queue fTags;
};

