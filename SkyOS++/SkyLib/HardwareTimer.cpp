// 
// Copyright 1998-2012 Jeff Bush
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// 

//
//	Interface to 8253 Programmable Interrupt Timer
//

#include "cpu_asm.h"
#include "InterruptHandler.h"
#include "Timer.h"

extern "C" int _outp(unsigned short, int);
extern "C" unsigned long _outpd(unsigned int, int);
extern "C" unsigned short _outpw(unsigned short, unsigned short);
extern "C" int _inp(unsigned short);
extern "C" unsigned short _inpw(unsigned short);
extern "C" unsigned long _inpd(unsigned int shor);

typedef InterruptStatus (*TimerCallback)();
const bigtime_t kPitClockRate = 1193180;
const bigtime_t kMaxTimerInterval = (bigtime_t) 0xffff * 1000000 / kPitClockRate;

class TimerDispatcher : public InterruptHandler {
public:
	TimerDispatcher(TimerCallback callback);
	InterruptStatus HandleInterrupt();

private:
	TimerCallback fCallback;
};

TimerDispatcher::TimerDispatcher(TimerCallback callback)
	:	fCallback(callback)
{
	ObserveInterrupt(0);
}

InterruptStatus TimerDispatcher::HandleInterrupt()
{
	return fCallback();
}

void HardwareTimerBootstrap(TimerCallback callback)
{
	_outp(0x30, 0x43);
	_outp(0, 0x40);
	_outp(0, 0x40);
	new TimerDispatcher(callback);
}

void SetHardwareTimer(bigtime_t relativeTimeout)
{
	bigtime_t nextEventClocks;
	if (relativeTimeout <= 0)
		nextEventClocks = 2;			
	else if (relativeTimeout < kMaxTimerInterval)
		nextEventClocks = relativeTimeout * kPitClockRate / 1000000;
	else
		nextEventClocks = 0xffff;

#ifndef SKY_EMULATOR
	_outp(0x30, 0x43);
	_outp(nextEventClocks & 0xff, 0x40);
	_outp((nextEventClocks >> 8) & 0xff, 0x40);
#endif
}
