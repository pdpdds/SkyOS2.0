/* 
** Copyright 2003, Travis Geiselbrecht. All rights reserved.
** Distributed under the terms of the NewOS License.
*/
#ifndef _KERNEL_TIME_H
#define _KERNEL_TIME_H

#include "windef.h"
// kernel args
typedef struct {
	// architecture specific
	int foo;
} kernel_args;
int time_init(kernel_args *ka);
int time_init2(kernel_args *ka); // should be called just before enabling interrupts
void time_tick(int tick_rate);

// microseconds since the system was booted
bigtime_t system_time(void);

// microseconds since the system was booted, accurate only to a few milliseconds
bigtime_t system_time_lores(void);

// usecs since Jan 1, 1AD
bigtime_t local_time(void);

#endif

