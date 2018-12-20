#pragma once

#include "stdint.h"
#include "skyramdiskentry.h"
#include "BasicStruct.h"
#include "skyramdisk_struct.h"


class SkyRamDisk 
{
private:
	RamDiskEntry* firstHeader;
	RamDiskEntry* root;
	uint32_t next_unused_id = 0;

public:
	/**
	 * Initializes the empty ramdisk. A ramdisk is never deleted, therefore
	 * there is no destructor.
	 */
	SkyRamDisk();

	/**
	 * Loads the ramdisk by creating actual {RamdiskEntry} objects. Reads the "module"
	 * and interprets it as a ramdisk filesystem
	 *
	 * @param module	the ramdisk multiboot module
	 */
	RamDiskEntry* Load(BootModule* module);

	/**
	 * Searches in the folder parent for a file/folder with the given name
	 *
	 * @param parent the parent folder to search through
	 * @param childName the name of the child to find
	 * @return the entry if found, else 0
	 */
	RamDiskEntry* FindChild(RamDiskEntry* node, const char* name);

	/**
	 * Searches for the entry at the given absolute path
	 *
	 * @param name	name of the entry to find
	 * @return the entry if found, else 0
	 */
	RamDiskEntry* FindAbsolute(const char* name);

	/**
	 * Searches for a child with of "node" with the given relative path
	 *
	 * @param node	the parent node
	 * @param path	the relative child path
	 * @return the entry if found, else 0
	 */
	RamDiskEntry* FindRelative(RamDiskEntry* node, const char* path);

	/**
	 * Returns the entry with the given "id"
	 *
	 * @param name the name
	 * @return the entry if it exists, else 0
	 */
	RamDiskEntry* FindById(uint32_t id);

	/**
	 * Returns the number of children that the entry with "id" has
	 *
	 * @param id	the entries id
	 * @return the number of children
	 */
	uint32_t GetChildCount(uint32_t id);

	/**
	 * Returns the child at "index" of the node "id"
	 *
	 * @param id	the parent entries id
	 * @param index	the index of the child
	 * @return the entry if found, else 0
	 */
	RamDiskEntry* GetChildAt(uint32_t id, uint32_t index);

	/**
	 * Returns the root.
	 */
	RamDiskEntry* GetRoot() const;
	RamDiskEntry* GetFirst() const;

	/**
	 *
	 */
	RamDiskEntry* CreateChild(RamDiskEntry* parent, char* filename);

};
