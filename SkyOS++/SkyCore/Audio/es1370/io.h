#ifndef _IO_H_
#define _IO_H_

typedef int int32;
typedef unsigned long long uint64;
typedef unsigned short uint16;
typedef unsigned short ushort;

#define false 0
#define true 1

#include "config.h"

uint8  es1370_reg_read_8(device_config *config, int regno);
uint16 es1370_reg_read_16(device_config *config, int regno);
uint32 es1370_reg_read_32(device_config *config, int regno);

void es1370_reg_write_8(device_config *config, int regno, uint8 value);
void es1370_reg_write_16(device_config *config, int regno, uint16 value);
void es1370_reg_write_32(device_config *config, int regno, uint32 value);

uint16 es1370_codec_read(device_config *config, int regno);
void es1370_codec_write(device_config *config, int regno, uint16 value);
	       
#endif
