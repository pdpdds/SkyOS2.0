#pragma once
#define NUM_CARDS 3
#define DEVNAME 32


typedef struct
{
	uint32	base;
	uint32	irq;
	uint32	type;
} device_config;
