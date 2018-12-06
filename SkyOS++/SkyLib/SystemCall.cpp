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
#include "AddressSpace.h"
#include "APC.h"
#include "Area.h"
#include "cpu_asm.h"
//#include "Dispatcher.h"
#include "FileDescriptor.h"
#include "FileSystem.h"
#include "HandleTable.h"
//#include "Image.h"
#include "KernelDebug.h"
#include "PageCache.h"
#include "stdio.h"
#include "string.h"
#include "syscall.h"
#include "SystemCall.h"
#include "Team.h"
#include "datastructure/Thread.h"
#include "_types.h"
#include "VNode.h"
#include "HandleTable.h"

static Semaphore gSleepSemaphore("sleep_sem", 0);

void thread_exit()
{
	Thread::GetRunningThread()->Exit();
}

extern "C" void ContextSwitch(unsigned int *oldEsp, unsigned int newEsp, unsigned int pdbr)
{
	__asm
	{		
		pushf
		push ebp
		push esi
		push edi
		push ebx
		mov eax, [esp + 24]			; Get location to save stack pointer at
		mov eax, esp					; Save old stack pointer
		mov eax, [esp + 32]			; Get new PDBR
		mov ebx, [esp + 28]			; Get new stack pounter
		cmp eax, 0xffffffff			; Need to change addr.space ?
		je skip_change_cr3				; If parameter was - 1, no
		mov cr3, eax					; Change address space
skip_change_cr3: 
		mov esp, ebx					; Switch to the new stack
		pop ebx
		pop edi
		pop esi
		pop ebp
		popf
		ret
	}
}


extern "C" void SwitchToUserMode(unsigned int _start, unsigned int user_stack)
{
	__asm
	{		
		mov eax, [esp + 4] 			; start address
		mov ebx, [esp + 8]			; stack
		mov cx, 0x20				; set up segment registers
		mov ds, cx
		mov es, cx
		mov fs, cx
		mov gs, cx

		; Set up a cross - protection level interrupt frame to jump to
		; the new thread.
		push 0x23					; User data segment
		push ebx					; User stack
		push (1 << 9) | 2			; User Flags(note that interrupts are on)
		push 0x1b					; User CS
		push eax					; EIP
		iret						; Jump to user space
		
	}
}

extern "C" int CopyUserInternal(void *dest, const void *src, unsigned int size, unsigned int *handler)
{
	__asm
	{
		
		push edi
		push esi
		push ebx
		mov ebx, [esp + 28]
		mov ebx, on_fault
		xor eax, eax				; Clear success flag
		mov ecx, [esp + 24]
		mov edi, [esp + 16]
		mov esi, [esp + 20]
		rep movsb
		mov eax, 1					; Set success flag
on_fault:
		mov ebx, 0					; Clear fault handler
		pop ebx
		pop esi
		pop edi
		ret				   
	}

	return 0;
}

inline Resource *GetResource(int handle, int type)
{
	return Thread::GetRunningThread()->GetTeam()->GetHandleTable()->GetResource(handle, type);
}

inline int OpenHandle(Resource *obj)
{
	return Thread::GetRunningThread()->GetTeam()->GetHandleTable()->Open(obj);
}
	
ssize_t write_pos(int handle, off_t offs, const void *in, size_t length)
{
	FileDescriptor *descriptor = static_cast<FileDescriptor*>(GetResource(handle, OBJ_FD));
	if (descriptor == 0)
		return E_BAD_HANDLE;

	size_t ret = descriptor->WriteAt(offs, in, length);
	descriptor->ReleaseRef();
	return ret;
}

ssize_t read_pos(int handle, off_t offs, void *out, size_t length)
{
	FileDescriptor *descriptor = static_cast<FileDescriptor*>(GetResource(handle, OBJ_FD));
	if (descriptor == 0)
		return E_BAD_HANDLE;

	ssize_t err = descriptor->ReadAt(offs, out, length);
	descriptor->ReleaseRef();
	return err;
}

int open(const char path[], int)
{
	VNode *node;
	int error = FileSystem::WalkPath(path, strlen(path), &node);
	if (error < 0)
		return error;

	FileDescriptor *desc;
	error = node->Open(&desc);
	if (error < 0) {
		node->ReleaseRef();
		return error;
	}

	desc->SetName(path);
	return OpenHandle(desc);
}


int sleep(bigtime_t timeout)
{
	gSleepSemaphore.Wait(timeout);
	return 0;
}