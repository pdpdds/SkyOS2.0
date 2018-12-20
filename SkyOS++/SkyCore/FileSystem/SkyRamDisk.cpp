#include "windef.h"
#include "skyramdisk.h"
#include "string.h"
#include "memory.h"
#include "SkyConsole.h"
/**
 * 
 */

int indexOf(const char* str, char c) {
	int pos = 0;
	while (*str) {
		if (*str == c) {
			return pos;
		}
		++pos;
		++str;
	}
	return -1;
}

SkyRamDisk::SkyRamDisk()
{
	root = 0;
	firstHeader = 0;
}

/**
 * 
 */
RamDiskEntry* SkyRamDisk::Load(BootModule* module)
{
	// Get the ramdisk location and its end from the multiboot info
	uint8_t* ramdisk = (uint8_t*) module->ModuleStart;
	uint32_t ramdiskEnd = module->ModuleEnd;

	// Create a root RamdiskEntry
	root = new RamDiskEntry;
	root->id = 0;

	// Initialize the header storage location
	firstHeader = 0;

	// Create the position
	uint32_t ramdiskPosition = 0;
	//SkyConsole::Print("0x%x 0x%x\n", ramdisk, ramdiskEnd);
	
	// Iterate through the ramdisk
	RamDiskEntry* currentHeader = 0;
	while ((uint32_t) (ramdisk + ramdiskPosition) < ramdiskEnd) {
		RamDiskEntry* header = new RamDiskEntry;
		header->next = 0;
		
		if (currentHeader == 0) {
			firstHeader = header;
			currentHeader = firstHeader;
		} else {
			currentHeader->next = header;
			currentHeader = header;
		}

		// Type (file/folder)
		uint8_t* typeptr = (uint8_t*) (ramdisk + ramdiskPosition);
		
		header->type = static_cast<RamDiskEntry_Type>(*typeptr);
	
		ramdiskPosition++;

		// ID
		uint32_t* idptr = (uint32_t*) (ramdisk + ramdiskPosition);
		header->id = *idptr;
		ramdiskPosition += 4;

		// parent ID
		uint32_t* parentidptr = (uint32_t*) (ramdisk + ramdiskPosition);
		header->parentid = *parentidptr;
		ramdiskPosition += 4;

		// Name
		uint32_t* namelengthptr = (uint32_t*) (ramdisk + ramdiskPosition);
		uint32_t namelength = *namelengthptr;
		ramdiskPosition += 4;

		uint8_t* nameptr = (uint8_t*) (ramdisk + ramdiskPosition);
		header->name = new char[namelength + 1];
		memcpy(header->name, nameptr, namelength);
		header->name[namelength] = 0;
		ramdiskPosition += namelength;

		// If its a file, load rest
		header->data_on_ramdisk = true;
		if (header->type == G_RAMDISK_ENTRY_TYPE_FILE) {
			// Data length
			uint32_t* datalengthptr = (uint32_t*) (ramdisk + ramdiskPosition);
			header->datalength = *datalengthptr;
			ramdiskPosition += 4;
			SkyConsole::Print("%s 0x%x\n", header->name, header->id);
			// Copy data
			header->data = (uint8_t*) (ramdisk + ramdiskPosition);
			ramdiskPosition += header->datalength;
		} else {
			header->datalength = 0;
			header->data = 0;
		}

		// start with unused ids after the last one
		if (header->id > next_unused_id) {
			next_unused_id = header->id + 1;
		}
	}
	
	return this->root;
}

/**
 * 
 */
RamDiskEntry* SkyRamDisk::FindChild(RamDiskEntry* parent, const char* childName)
{

	RamDiskEntry* current = firstHeader;
	do {
		if (current->parentid == parent->id && strcmp(current->name, childName) == 0) 
		{
			return current;
		}
	} while ((current = current->next) != 0);

	return 0;
}

/**
 * 
 */
RamDiskEntry* SkyRamDisk::FindById(uint32_t id)
{
	RamDiskEntry* foundNode = 0;

	if (id == 0) {
		return root;
	}

	RamDiskEntry* currentNode = firstHeader;
	while (currentNode != 0) {
		if (currentNode->id == id) {
			foundNode = currentNode;
			break;
		}

		currentNode = currentNode->next;
	}

	return foundNode;
}

/**
 * 
 */
RamDiskEntry* SkyRamDisk::FindAbsolute(const char* path)
{

	return FindRelative(root, path);
}

/**
 * Searches for the entry at the given the relative path to the given node.
 */
RamDiskEntry* SkyRamDisk::FindRelative(RamDiskEntry* node, const char* path)
{
	char buf[G_RAMDISK_MAXIMUM_PATH_LENGTH];
	uint32_t pathLen = strlen(path);
	memcpy(buf, path, pathLen);
	buf[pathLen] = 0;

	RamDiskEntry* currentNode = node;
	while (strlen(buf) > 0) {
		int slashIndex = indexOf(buf, '/');
		if (slashIndex >= 0) {
			// Set current node to next layer
			if (slashIndex > 0) {
				char childpath[G_RAMDISK_MAXIMUM_PATH_LENGTH];
				memcpy(childpath, buf, slashIndex);
				childpath[slashIndex] = 0;

				currentNode = FindChild(currentNode, childpath);
			}

			// Cut off layer
			uint32_t len = strlen(buf) - (slashIndex + 1);
			memcpy(buf, &buf[slashIndex + 1], len);
			buf[len] = 0;
		} else {
			// Reached the last node, find the child
			currentNode = FindChild(currentNode, buf);
			break;
		}
	}
	return currentNode;
}

/**
 * 
 */
uint32_t SkyRamDisk::GetChildCount(uint32_t id) 
{

	uint32_t count = 0;

	RamDiskEntry* currentNode = firstHeader;
	while (currentNode != 0) {
		if (currentNode->parentid == id) {
			++count;
		}
		currentNode = currentNode->next;
	}

	return count;
}

/**
 * 
 */
RamDiskEntry* SkyRamDisk::GetChildAt(uint32_t id, uint32_t index)
{

	uint32_t pos = 0;

	RamDiskEntry* currentNode = firstHeader;
	while (currentNode != 0) {
		if (currentNode->parentid == id) {
			if (pos == index) {
				return currentNode;
			}

			++pos;
		}
		currentNode = currentNode->next;
	}

	return 0;
}

/**
 *
 */
RamDiskEntry* SkyRamDisk::GetRoot() const
{
	return root;
}

RamDiskEntry* SkyRamDisk::GetFirst() const
{
	return firstHeader;
}

/**
 *
 */
RamDiskEntry* SkyRamDisk::CreateChild(RamDiskEntry* parent, char* filename)
{

	RamDiskEntry* new_node = new RamDiskEntry();
	new_node->next = firstHeader;
	firstHeader = new_node;

	// copy name
	int namelen = strlen(filename);
	new_node->name = new char[namelen + 1];
	strcpy(new_node->name, filename);

	new_node->type = G_RAMDISK_ENTRY_TYPE_FILE;
	new_node->id = next_unused_id++;
	new_node->parentid = parent->id;

	// set empty buffer
	new_node->data = nullptr;
	new_node->datalength = 0;
	new_node->not_on_rd_buffer_length = 0;
	new_node->data_on_ramdisk = false;

	return new_node;
}
