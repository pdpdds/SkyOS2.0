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
#include "Image.h"
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
#include "PlatformAPI.h"

int bad_syscall(...);

SystemCallInfo systemCallTable[] = {
	/*{ (CallHook)sleep, 2 },
	{ (CallHook)_serial_print, 1 },
	{ (CallHook)spawn_thread, 4 },
	{ (CallHook)thread_exit, 0 },
	{ (CallHook)create_sem, 2 },
	{ (CallHook)acquire_sem, 3 },
	{ (CallHook)release_sem, 2 },
	{ (CallHook)think, 0 },
	{ (CallHook)open, 3 },
	{ (CallHook)close_handle, 1 },
	{ (CallHook)stat, 2 },
	{ (CallHook)ioctl, 3 },
	{ (CallHook)write, 3 },
	{ (CallHook)read, 3 },
	{ (CallHook)mkdir, 2 },
	{ (CallHook)rmdir, 1 },
	{ (CallHook)rename, 2 },
	{ (CallHook)readdir, 3 },
	{ (CallHook)rewinddir, 1 },
	{ (CallHook)lseek, 3 },
	{ (CallHook)exec, 1 },
	{ (CallHook)create_area, 6 },
	{ (CallHook)clone_area, 5 },
	{ (CallHook)delete_area, 1 },
	{ (CallHook)resize_area, 2 },
	{ (CallHook)wait_for_multiple_objects, 5 },
	{ (CallHook)bad_syscall, 0 },
	{ (CallHook)kill_thread, 1 },
	{ (CallHook)read_pos, 5 },
	{ (CallHook)write_pos, 5 },
	{ (CallHook)chdir, 1 },
	{ (CallHook)getcwd, 2 },
	{ (CallHook)mount, 5 },
	{ (CallHook)map_file, 6 },*/
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
	{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },{ bad_syscall,0 },
};

static Semaphore gSleepSemaphore("sleep_sem", 0);

void thread_exit()
{
	Thread::GetRunningThread()->Exit();
}

/*extern "C" void ContextSwitch(unsigned int *oldEsp, unsigned int newEsp, unsigned int pdbr)
{
	

	static int i = 0;

	if (i == 3)
	{
		printf("count : %d, %x, %x, %x\n",i, *oldEsp, newEsp, pdbr);
		
	}
	i++;

	unsigned int aaab = (unsigned int)&oldEsp;
	__asm
	{			
		pushfd
		push ebp
		push esi
		push edi
		push ebx		
		mov eax, aaab; Get location to save stack pointer at
		mov [eax], esp					; Save old stack pointer
		mov eax, pdbr				; Get new PDBR
		mov ebx, newEsp				; Get new stack pounter		
		cmp eax, 0xffffffff			; Need to change addr.space ?
		je skip_change_cr3				; If parameter was - 1, no			
		mov cr3, eax					; Change address space
skip_change_cr3: 
		mov esp, ebx; Switch to the new stack
		pop ebx
		pop edi
		pop esi
		pop ebp
		popfd
		ret
	}
}*/


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

status_t close_handle(int handle)
{
	Resource *resource = static_cast<FileDescriptor*>(GetResource(handle, OBJ_ANY));
	if (resource == 0)
		return E_BAD_HANDLE;

	Thread::GetRunningThread()->GetTeam()->GetHandleTable()->Close(handle);
	resource->ReleaseRef();
	return 0;
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


extern "C" int sleep(bigtime_t timeout)
{
#ifdef SKY_EMULATOR
	platformAPI._processInterface.sky_ksleep(timeout/1000);
#else
	gSleepSemaphore.Wait(timeout);
#endif
	return 0;
}

int bad_syscall(...)
{
	printf("invalid syscall");
	return E_INVALID_OPERATION;
}

status_t mount(const char device[], const char dir[], const char type[], int,
	char*)
{
	int error;
	int devfd = -1;
	if (strlen(device) > 0) {
		devfd = open(device, 0);
		if (devfd < 0) {
			printf("mount: error opening device\n");
			return E_NO_SUCH_FILE;
		}
	}

	VNode *node;
	error = FileSystem::WalkPath(dir, strlen(dir), &node);
	if (error < 0) {
		printf("mount: mount point does not exist\n");
		close_handle(devfd);
		return error;
	}

	if (node->GetCoveredBy() != 0) {
		// Attempting to re-mount an already mounted directory
		printf("mount: filesystem already mounted at this point\n");
		node->ReleaseRef();
		close_handle(devfd);
		return E_NOT_ALLOWED;
	}

	FileSystem *fs;
	error = FileSystem::InstantiateFsType(type, devfd, &fs);
	if (error < 0) {
		node->ReleaseRef();
		close_handle(devfd);
		return error;
	}

	node->CoverWith(fs);
	fs->Cover(node);
	return 0;
}

status_t mkdir(const char path[], mode_t)
{
	VNode *dir;
	char entry[256];
	int error;

	error = FileSystem::GetDirEntry(path, strlen(path), entry, 256, &dir);
	if (error < 0)
		return error;

	error = dir->MakeDir(entry, strlen(entry));
	dir->ReleaseRef();
	return error;
}

int CreateFileArea(const char name[], const char path[], unsigned int va, off_t offset,
	size_t size, int flags, PageProtection prot, Team &team)
{
	VNode *node;
	int error;

	if (offset % PAGE_SIZE) {
		printf("Unaligned file offset for area\n");
		return E_INVALID_OPERATION; // File offset must be page aligned
	}

	error = FileSystem::WalkPath(path, strlen(path), &node);
	if (error < 0) {
		printf("map_file: file not found\n");
		return error;
	}

	PageCache *cache = node->GetPageCache();

	// If this is a private area, construct a copy cache
	if ((flags & MAP_PRIVATE) != 0) {
		cache = new PageCache(0, cache);
		if (cache == 0) {
			node->ReleaseRef();
			return E_NO_MEMORY;
		}
	}

	// It is important that CreateArea not incur a fault!		
	char nameCopy[OS_NAME_LENGTH];
	if (!CopyUser(nameCopy, name, OS_NAME_LENGTH))
		return E_BAD_ADDRESS;

	Area *area = team.GetAddressSpace()->CreateArea(nameCopy, size, AREA_NOT_WIRED,
		prot, cache, offset, va, flags);

	if (area == 0) {
		printf("CreateArea failed\n");
		node->ReleaseRef();
		return E_ERROR;
	}

	return team.GetHandleTable()->Open(area);
}

status_t exec(const char path[])
{
	Image image;
	if (image.Open(path) != E_NO_ERROR)
		return E_NO_SUCH_FILE;

	Team *newTeam = new Team(path);
	if (newTeam == 0)
		return E_NO_MEMORY;

	if (image.Load(*newTeam) != E_NO_ERROR) {
		printf("error loading image\n");
		newTeam->ReleaseRef();
		return E_NOT_IMAGE;
	}

	const char *filename = path + strlen(path);
	while (filename > path && *filename != '/')
		filename--;

	char appName[OS_NAME_LENGTH];
	snprintf(appName, OS_NAME_LENGTH, "%.12s thread", filename + 1);

	thread_start_t entry = reinterpret_cast<thread_start_t>(image.GetEntryAddress());
	Thread *child = new Thread(appName, newTeam, entry, 0);
	if (child == 0) {
		newTeam->ReleaseRef();
		return E_NO_MEMORY;
	}

	return E_NO_ERROR;	// Maybe return a handle to the team, but then it would
						// have to be explicitly closed
}
extern "C" void ExecThread(const char name[], thread_start_t startAddress, void *param, int priority)
{
	new Thread(name, Thread::GetRunningThread()->GetTeam(), startAddress, param, 1);

#ifdef SKY_EMULATOR	
	platformAPI._processInterface.sky_kcreate_thread_from_memory(1, (LPTHREAD_START_ROUTINE)startAddress, param);
#endif
	
}