#ifndef DRIVER_H
#define DRIVER_H

//#include "driver_settings.h"
//#include "KernelExport.h"
//#include "Drivers.h"

#include "windef.h"
#include "stdint.h"
#include "string.h"

#ifdef COMPILE_FOR_R5
        #define DEVFS_PATH	"audio/multi"
        #include <multi_audio.h>
#else
        #define DEVFS_PATH	"audio/hmulti"
        #include "hmulti_audio.h"
#endif

#define STRMINBUF		2
#define STRMAXBUF		2
#define DEFAULT_FRAMESPERBUF	512

#define SB16_MULTI_CONTROL_FIRSTID       1024
#define SB16_MULTI_CONTROL_MASTERID      0

typedef struct {
	int		running;
	spinlock	lock;
	int bits;

	void*		buffers[STRMAXBUF];
	uint32_t		num_buffers;
	uint32_t		num_channels;
	uint32_t		sample_size;
	uint32_t		sampleformat;
	uint32_t		samplerate;
	uint32_t		buffer_length;
	sem_id		buffer_ready_sem;
	uint32_t		frames_count;
	uint32_t		buffer_cycle;
	bigtime_t	real_time;
} sb16_stream_t;

typedef struct {
	int port, irq, dma8, dma16, midiport;
	int opened;

	sb16_stream_t playback_stream;
	sb16_stream_t record_stream;
} sb16_dev_t;

extern device_hooks driver_hooks;

status_t sb16_hw_init(sb16_dev_t* dev);
void sb16_hw_stop(sb16_dev_t* dev);
void sb16_hw_uninit(sb16_dev_t* dev);

status_t sb16_stream_setup_buffers(sb16_dev_t* dev, sb16_stream_t* s, const char* desc);
status_t sb16_stream_start(sb16_dev_t* dev, sb16_stream_t* s);
status_t sb16_stream_stop(sb16_dev_t* dev, sb16_stream_t* s);
void sb16_stream_buffer_done(sb16_stream_t* stream);

status_t multi_audio_control(void* cookie, uint32_t op, void* arg, size_t len);

#endif /* DRIVER_H */

