#pragma once

#define PAGE_SIZE 4096
#define MEGA_BYTES 1048576
#define KILO_BYTES 1024

#define KERNEL32_NAME		"SkyOS32.EXE"
#define KERNEL64_NAME		"SKYOS64_SYS"

#define PAGE_ALIGN_DOWN(value)				((value) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_UP(value)				(((value) & (PAGE_SIZE - 1)) ? (PAGE_ALIGN_DOWN((value)) + PAGE_SIZE) : (value))

const unsigned int kUserBase = 0x1000;
const unsigned int kUserTop = 0xbfffffff;
const unsigned int kKernelBase = 0xc0000000;
//const unsigned int kKernelDataBase = (((unsigned)&__data_start + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));
//const unsigned int kKernelDataTop = ((((unsigned)&_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1)) - 1);
const unsigned int kBootStackBase = 0xc0100000;
const unsigned int kBootStackTop = 0xc0103fff;
const unsigned int kHeapBase = 0xc0104000;
const unsigned int kHeapTop = 0xc01fffff;
const unsigned int kIOAreaBase = 0xe4000000;
const unsigned int kIOAreaTop = 0xe7ffffff;
const unsigned int kKernelTop = 0xffffffff;
const unsigned int kAddressSpaceTop = 0xffffffff;