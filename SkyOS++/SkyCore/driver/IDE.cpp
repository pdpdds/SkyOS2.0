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

#include "cpu_asm.h"
#include "KernelDebug.h"
#include "Device.h"
#include "InterruptHandler.h"
#include "Semaphore.h"
#include "stdio.h"
#include "syscall.h"
#include "types.h"
#include "intrinsic.h"
#include "SystemAPI.h"

const unsigned int kStatusError = 1;
const int kBlockSize = 512;

class Ide : public Device, public InterruptHandler {
public:
	Ide();
	virtual ~Ide();
	virtual int Read(off_t offset, void *ptr, size_t size);
	virtual int Write(off_t offset, const void *ptr, size_t size);
	
private:
	int SendCommand(unsigned int op, int sectorCount, int cylinder, int head,
		int sector, bool wait = true);
	int SendCommand(unsigned int op);
	virtual InterruptStatus HandleInterrupt();
	void WaitForController() const;
	void ResetController();	
	int BlockIo(unsigned int lba, void *, bool read);
	int ProbeGeometry();
	int Recalibrate();
	void PrintStatus();

	unsigned int fBasePort;
	int fStatus;
	Semaphore fCompletionSem;
	char fBuffer[kBlockSize];
	int fHeadCount;
	int fSectorsPerTrack;
	int fCylinderCount;
};

Ide::Ide()
	:	fBasePort(0x1f0),
		fCompletionSem("Ide:command_complete", 0)
{
	ObserveInterrupt(14);
	ResetController();
	Recalibrate();
	ProbeGeometry();
}

Ide::~Ide()
{
	IgnoreInterrupts();
}

int Ide::Read(off_t offset, void *ptr, size_t size)
{
	ASSERT((size % kBlockSize) == 0);
	ASSERT((offset % kBlockSize) == 0);
	
	int error = BlockIo(offset / kBlockSize, ptr, true);
	if (error != 0)
		return error;
		
	return size;
}

int Ide::Write(off_t offset, const void *ptr, size_t size)
{
	ASSERT((size % kBlockSize) == 0);
	ASSERT((offset % kBlockSize) == 0);

	for (size_t written = 0; written < size; written += kBlockSize) {	
		int error = BlockIo((offset + written) / kBlockSize, const_cast<char*>(
			reinterpret_cast<const char*>(ptr)) + written, false);
		if (error != 0)
			return error;
	}
		
	return size;
}

int Ide::SendCommand(unsigned int op, int sectorCount, int cylinder, int head,
	int sector, bool wait)
{
	if (head > 7)
		OutPortByte(0x3f6, 0x8);

	OutPortByte(fBasePort + 2, sectorCount);
	WaitForController();
	OutPortByte(fBasePort + 3, sector);
	WaitForController();
	OutPortByte(fBasePort + 4, (cylinder >> 8) & 0xff);
	WaitForController();
	OutPortByte(fBasePort + 5, cylinder & 0xff);
	WaitForController();
	OutPortByte(fBasePort + 6, (head & 0xf) | 32 | 128);
	WaitForController();
	OutPortByte(fBasePort + 7, op);

	if (!wait)
		return 0;
		
	while (fCompletionSem.Wait() == E_INTERRUPTED)
		;

	return (fStatus & kStatusError) ? E_IO : E_NO_ERROR;
}

int Ide::SendCommand(unsigned int op)
{
	WaitForController();
	OutPortByte(fBasePort + 7, op);
	while (fCompletionSem.Wait() == E_INTERRUPTED)
		;
		
	return 0;
}

InterruptStatus Ide::HandleInterrupt()
{
	fStatus = InPortByte(fBasePort + 7);
	fCompletionSem.Release(1, false);
	return kReschedule;
}

int Ide::BlockIo(unsigned int lba, void *data, bool read)
{
	const int cylinder = lba / (fHeadCount * fSectorsPerTrack);
	const int head = (lba / fSectorsPerTrack) % fHeadCount;
	const int sector = lba % fSectorsPerTrack + 1;

	if (cylinder >= fCylinderCount)
		return E_IO;

	for (int retry = 0; retry < 2; retry++) {
		if (read) {
			if (SendCommand(0x20, 1, cylinder, head, sector) < 0)
				Recalibrate();
			else {
				read_io_str_16(fBasePort, reinterpret_cast<short*>(data), 256);
				break;
			}
		} else {
			// Note that write doesn't interrupt for first sector
			if (SendCommand(0x30, 1, cylinder, head, sector, false) < 0) {
				Recalibrate();
			} else {
				WaitForController();
				while ((InPortByte(fBasePort + 7) & 8) == 0)	// Wait for DRQ
					;
					
				write_io_str_16(fBasePort, reinterpret_cast<short*>(data), 256);
				fCompletionSem.Wait();
				break;
			}
		}
	}

	return (fStatus & 0x21) == 0 ? 0 : E_IO;
}

void Ide::WaitForController() const
{
	while (InPortByte(fBasePort + 7) & 0x80)
		;
}

void Ide::ResetController()
{
	OutPortByte(0x3f6, 4);	// reset controller
	sleep(40000);
	OutPortByte(0x3f6, 8);	// clear reset bit, and set large disk flag
}

int Ide::ProbeGeometry()
{
	if (SendCommand(0xec) < 0)
		return E_IO;
		
	short *geometry = new short[kBlockSize / 2];
	read_io_str_16(fBasePort, geometry, kBlockSize);
	fCylinderCount = geometry[54];
	fHeadCount = geometry[55];
	fSectorsPerTrack = geometry[56];
	delete [] geometry;

	printf("Ide drive initialized: heads %d sect/track %d\n",
		fHeadCount, fSectorsPerTrack);

	return E_NO_ERROR;
}

int Ide::Recalibrate()
{
	if (SendCommand(0x10) < 0)
		return E_IO;
		
	return E_NO_ERROR;
}

void Ide::PrintStatus()
{
	printf("Command status:\n");
	if (fStatus & 2) printf("  disk index has just passed\n");
	if (fStatus & 4) printf("  Correctable data error\n");
	if (fStatus & 8) printf("  Data can be transferred\n");
	if (fStatus & 16) printf("  seek complete\n");
	if (fStatus & 32) printf("  Write fault\n");
	if (fStatus & 64) printf("  Drive is ready\n");
	if (fStatus & 64) printf("  Drive is busy\n");
	
	if (fStatus & kStatusError) {
		printf("The following error(s) occured:\n");
		unsigned int error = InPortByte(fBasePort + 1);
		if (error & 1) printf("  data address mark not found\n");
		if (error & 2) printf("  track zero not found\n");
		if (error & 4) printf("  command aborted\n");
		if (error & 8) printf("  medium change required\n");
		if (error & 16) printf("  ID mark not found\n");
		if (error & 32) printf("  medium changed\n");
		if (error & 64) printf("  Uncorrectable data error\n");
		if (error & 128) printf("  Sector marked bad by host\n");
	}
}

Device* IdeInstantiate()
{
	return new Ide;
}


