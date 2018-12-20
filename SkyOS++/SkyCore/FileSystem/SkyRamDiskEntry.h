#pragma once 
#include "skyramdisk.h"
#include "skyramdisk_struct.h"

/**
 * Struct of a ramdisk entry
 */
struct RamDiskEntry 
{
	RamDiskEntry* next;
	RamDiskEntry_Type type;
	uint32_t id;
	uint32_t parentid;

	char* name;

	uint32_t datalength;
	uint8_t* data;

	bool data_on_ramdisk;
	uint32_t not_on_rd_buffer_length;
};