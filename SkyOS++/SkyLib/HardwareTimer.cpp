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

extern "C" void
__declspec(naked)
_alldiv()
{
	/* *INDENT-OFF* */
	__asm {
		push        edi
		push        esi
		push        ebx
		xor         edi, edi
		mov         eax, dword ptr[esp + 14h]
		or eax, eax
		jge         L1
		inc         edi
		mov         edx, dword ptr[esp + 10h]
		neg         eax
		neg         edx
		sbb         eax, 0
		mov         dword ptr[esp + 14h], eax
		mov         dword ptr[esp + 10h], edx
		L1 :
		mov         eax, dword ptr[esp + 1Ch]
			or eax, eax
			jge         L2
			inc         edi
			mov         edx, dword ptr[esp + 18h]
			neg         eax
			neg         edx
			sbb         eax, 0
			mov         dword ptr[esp + 1Ch], eax
			mov         dword ptr[esp + 18h], edx
			L2 :
		or eax, eax
			jne         L3
			mov         ecx, dword ptr[esp + 18h]
			mov         eax, dword ptr[esp + 14h]
			xor edx, edx
			div         ecx
			mov         ebx, eax
			mov         eax, dword ptr[esp + 10h]
			div         ecx
			mov         edx, ebx
			jmp         L4
			L3 :
		mov         ebx, eax
			mov         ecx, dword ptr[esp + 18h]
			mov         edx, dword ptr[esp + 14h]
			mov         eax, dword ptr[esp + 10h]
			L5 :
			shr         ebx, 1
			rcr         ecx, 1
			shr         edx, 1
			rcr         eax, 1
			or ebx, ebx
			jne         L5
			div         ecx
			mov         esi, eax
			mul         dword ptr[esp + 1Ch]
			mov         ecx, eax
			mov         eax, dword ptr[esp + 18h]
			mul         esi
			add         edx, ecx
			jb          L6
			cmp         edx, dword ptr[esp + 14h]
			ja          L6
			jb          L7
			cmp         eax, dword ptr[esp + 10h]
			jbe         L7
			L6 :
		dec         esi
			L7 :
		xor         edx, edx
			mov         eax, esi
			L4 :
		dec         edi
			jne         L8
			neg         edx
			neg         eax
			sbb         edx, 0
			L8 :
			pop         ebx
			pop         esi
			pop         edi
			ret         10h
	}
	/* *INDENT-ON* */
}

extern "C" void
__declspec(naked)
_allshr()
{
	/* *INDENT-OFF* */
	__asm {
		cmp         cl, 40h
		jae         RETZERO
		cmp         cl, 20h
		jae         MORE32
		shrd        eax, edx, cl
		sar         edx, cl
		ret
		MORE32 :
		mov         eax, edx
			xor         edx, edx
			and         cl, 1Fh
			sar         eax, cl
			ret
			RETZERO :
		xor         eax, eax
			xor         edx, edx
			ret
	}
	/* *INDENT-ON* */
}

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

	_outp(0x30, 0x43);
	_outp(nextEventClocks & 0xff, 0x40);
	_outp((nextEventClocks >> 8) & 0xff, 0x40);
}
