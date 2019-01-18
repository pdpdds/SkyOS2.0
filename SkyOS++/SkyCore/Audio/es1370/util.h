#ifndef _UTIL_H_
#define _UTIL_H_

#include <KernelExport.h>

area_id alloc_mem(void **phy, void **log, size_t size, const char *name);
area_id map_mem(void **log, void *phy, size_t size, const char *name);

cpu_status lock(void);
void unlock(cpu_status status);

extern spinlock	slock;

#endif
