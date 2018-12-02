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

		/*
		pushf
		pushl %ebp
		pushl %esi
		pushl %edi
		pushl %ebx
		movl 24(%esp), %eax			# Get location to save stack pointer at
		movl %esp, (%eax)			# Save old stack pointer
		movl 32(%esp), %eax			# Get new PDBR
		movl 28(%esp), %ebx			# Get new stack pounter
		cmpl $0xffffffff, %eax		# Need to change addr.space ?
		je skip_change_cr3 			# If parameter was - 1, no
		movl %eax, %cr3				# Change address space
		skip_change_cr3 : movl %ebx, %esp				# Switch to the new stack
						  popl %ebx
						  popl %edi
						  popl %esi
						  popl %ebp
						  popf
						  ret*/
	}
}


extern "C" void SwitchToUserMode(unsigned int _start, unsigned int user_stack)
{
	__asm
	{
		/*
		movl 4(%esp), %eax			# start address
		movl 8(%esp), %ebx			# stack
		movw $0x20, %cx				# set up segment registers
		movw %cx, %ds
		movw %cx, %es
		movw %cx, %fs
		movw %cx, %gs

		# Set up a cross - protection level interrupt frame to jump to
		# the new thread.
		pushl $0x23					# User data segment
		pushl %ebx					# User stack
		pushl $(1 << 9) | 2			# User Flags(note that interrupts are on)
		pushl $0x1b					# User CS
		pushl %eax 					# EIP
		iret						# Jump to user space
		*/
	}
}

extern "C" int CopyUserInternal(void *dest, const void *src, unsigned int size, unsigned int *handler)
{
	__asm
	{
		/*
		pushl %edi
		pushl %esi
		pushl %ebx
		movl 28(%esp), %ebx
		movl $on_fault, (%ebx)
		xorl %eax, %eax				# Clear success flag
		movl 24(%esp), %ecx
		movl 16(%esp), %edi
		movl 20(%esp), %esi
		rep
		movsb
		movl $1, %eax				# Set success flag
		on_fault : movl $0, (%ebx)				# Clear fault handler
				   popl %ebx
				   popl %esi
				   popl %edi
				   ret
				   */
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