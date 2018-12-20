#pragma once

#define PAGE_SIZE 4096
#define MEGA_BYTES 1048576
#define KILO_BYTES 1024

#define KERNEL32_NAME		"SkyOS32.EXE"
#define KERNEL64_NAME		"SKYOS64_SYS"

#define PAGE_ALIGN_DOWN(value)				((value) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_UP(value)				(((value) & (PAGE_SIZE - 1)) ? (PAGE_ALIGN_DOWN((value)) + PAGE_SIZE) : (value))