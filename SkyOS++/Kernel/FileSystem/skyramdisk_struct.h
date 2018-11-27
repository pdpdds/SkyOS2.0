#ifndef __GHOST_SYS_RAMDISK__
#define __GHOST_SYS_RAMDISK__

#include "windef.h"

#ifdef __cplusplus
extern "C" {
#endif

// ramdisk spawning status codes
typedef uint8_t g_ramdisk_spawn_status;
#define G_RAMDISK_SPAWN_STATUS_SUCCESSFUL			1
#define G_RAMDISK_SPAWN_STATUS_FAILED_NOT_FOUND		2
#define G_RAMDISK_SPAWN_STATUS_FAILED_NOT_VALID		3
#define G_RAMDISK_SPAWN_STATUS_FAILED_NOT_PERMITTED	4

/**
 * Maximum length of paths within the ramdisk
 */
#define G_RAMDISK_MAXIMUM_PATH_LENGTH				512

// types of ramdisk entries
typedef char RamDiskEntry_Type;
#define G_RAMDISK_ENTRY_TYPE_UNKNOWN	-1
#define G_RAMDISK_ENTRY_TYPE_FOLDER		0
#define G_RAMDISK_ENTRY_TYPE_FILE		1

/**
 * Ramdisk entry information struct used within system calls
 */
#pragma pack (push, 1)
typedef struct {
	RamDiskEntry_Type type;
	char name[512];
	unsigned int length;
} g_ramdisk_entry_info;
#pragma pack (pop)
#ifdef __cplusplus
}
#endif

#endif
