#include <string.h>

//#define DEBUG 2

#include "util.h"

//spinlock slock = B_SPINLOCK_INITIALIZER;

uint32 round_to_pagesize(uint32 size);


cpu_status
lock(void)
{
	cpu_status status;
	//status = disable_interrupts();
	//acquire_spinlock(&slock);
	return status;
}


void
unlock(cpu_status status)
{
	//release_spinlock(&slock);
	//restore_interrupts(status);
}


uint32
round_to_pagesize(uint32 size)
{
	return (size + B_PAGE_SIZE - 1) & ~(B_PAGE_SIZE - 1);
}


area_id
alloc_mem(void **phy, void **log, size_t size, const char *name)
{
// TODO: phy should be phys_addr_t*!
	//physical_entry pe;
	//void * logadr;
	area_id areaid;
	/*status_t rv;

	LOG(("allocating %d bytes for %s\n",size,name));

	size = round_to_pagesize(size);
	areaid = create_area(name, &logadr, B_ANY_KERNEL_ADDRESS, size,
		B_32_BIT_CONTIGUOUS, B_READ_AREA | B_WRITE_AREA);
		// TODO: The rest of the code doesn't deal correctly with physical
		// addresses > 4 GB, so we have to force 32 bit addresses here.
	if (areaid < B_OK) {
		PRINT(("couldn't allocate area %s\n",name));
		return B_ERROR;
	}
	rv = get_memory_map(logadr, size, &pe, 1);
	if (rv < B_OK) {
		delete_area(areaid);
		PRINT(("couldn't map %s\n", name));
		return B_ERROR;
	}
	memset(logadr, 0, size);
	if (log)
		*log = logadr;
	if (phy)
		*phy = (void*)(addr_t)pe.address;
	LOG(("area = %d, size = %d, log = %#08X, phy = %#08X\n", areaid, size,
		logadr, pe.address));*/
	return areaid;
}


/* This is not the most advanced method to map physical memory for io access.
 * Perhaps using B_ANY_KERNEL_ADDRESS instead of B_ANY_KERNEL_BLOCK_ADDRESS
 * makes the whole offset calculation and relocation obsolete. But the code
 * below does work, and I can't test if using B_ANY_KERNEL_ADDRESS also works.
 */
area_id
map_mem(void **log, void *phy, size_t size, const char *name)
{
	//uint32 offset;
	//void *phyadr;
	//void *mapadr;
	area_id area;

	/*LOG(("mapping physical address %p with %#x bytes for %s\n",phy,size,name));

	offset = (uint32)phy & (B_PAGE_SIZE - 1);
	phyadr = phy - offset;
	size = round_to_pagesize(size + offset);
	area = map_physical_memory(name, (addr_t)phyadr, size,
		B_ANY_KERNEL_BLOCK_ADDRESS, B_READ_AREA | B_WRITE_AREA, &mapadr);
	*log = mapadr + offset;

	LOG(("physical = %p, logical = %p, offset = %#x, phyadr = %p, mapadr = %p, size = %#x, area = %#x\n",
		phy, *log, offset, phyadr, mapadr, size, area));*/

	return area;
}
