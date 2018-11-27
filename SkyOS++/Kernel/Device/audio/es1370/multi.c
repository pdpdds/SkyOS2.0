#include <driver_settings.h>
#include <OS.h>
//#include <MediaDefs.h>
#include <string.h>
#include "Errors.h"
//#include <strings.h>
#include "hmulti_audio.h"
#include "multi.h"
#include "ac97.h"
#include "memory.h"
extern void printf(const char* str, ...);
//#define DEBUG 1

//#include "debug.h"
#include "es1370.h"
#include "util.h"
#include "io.h"

static void	
es1370_ac97_get_mix(void *card, const void *cookie, int32 type, float *values) {
	es1370_dev *dev = (es1370_dev*)card;
	ac97_source_info *info = (ac97_source_info *)cookie;
	uint16 value, mask;
	float gain;
	
	switch(type) {
		case B_MIX_GAIN:
			value = es1370_codec_read(&dev->config, info->reg);
			//PRINT(("B_MIX_GAIN value : %u\n", value));
			if (info->type & B_MIX_STEREO) {
				mask = ((1 << (info->bits + 1)) - 1) << 8;
				gain = ((value & mask) >> 8) * info->granularity;
				if (info->polarity == 1)
					values[0] = info->max_gain - gain;
				else
					values[0] = gain - info->min_gain;
				
				mask = ((1 << (info->bits + 1)) - 1);
				gain = (value & mask) * info->granularity;
				if (info->polarity == 1)
					values[1] = info->max_gain - gain;
				else
					values[1] = gain - info->min_gain;
			} else {
				mask = ((1 << (info->bits + 1)) - 1);
				gain = (value & mask) * info->granularity;
				if (info->polarity == 1)
					values[0] = info->max_gain - gain;
				else
					values[0] = gain - info->min_gain;
			}
			break;
		case B_MIX_MUTE:
			mask = ((1 << 1) - 1) << 15;
			value = es1370_codec_read(&dev->config, info->reg);
			//PRINT(("B_MIX_MUTE value : %u\n", value));
			value &= mask;
			values[0] = ((value >> 15) == 1) ? 1.0 : 0.0;
			break;
		case B_MIX_MICBOOST:
			mask = ((1 << 1) - 1) << 6;
			value = es1370_codec_read(&dev->config, info->reg);
			//PRINT(("B_MIX_MICBOOST value : %u\n", value));
			value &= mask;
			values[0] = ((value >> 6) == 1) ? 1.0 : 0.0;
			break;
		case B_MIX_MUX:
			mask = ((1 << 3) - 1);
			value = es1370_codec_read(&dev->config, AC97_RECORD_SELECT);
			value &= mask;
			//PRINT(("B_MIX_MUX value : %u\n", value));
			values[0] = (float)value;
			break;
	}
}

static void	
es1370_ac97_set_mix(void *card, const void *cookie, int32 type, float *values) {
	es1370_dev *dev = (es1370_dev*)card;
	ac97_source_info *info = (ac97_source_info *)cookie;
	uint16 value, mask;
	float gain;
	
	switch(type) {
		case B_MIX_GAIN:
			value = es1370_codec_read(&dev->config, info->reg);
			if (info->type & B_MIX_STEREO) {
				mask = ((1 << (info->bits + 1)) - 1) << 8;
				value &= ~mask;
				
				if (info->polarity == 1)
					gain = info->max_gain - values[0];
				else
					gain =  values[0] - info->min_gain;
				value |= ((uint16)(gain	/ info->granularity) << 8) & mask;
				
				mask = ((1 << (info->bits + 1)) - 1);
				value &= ~mask;
				if (info->polarity == 1)
					gain = info->max_gain - values[1];
				else
					gain =  values[1] - info->min_gain;
				value |= ((uint16)(gain / info->granularity)) & mask;
			} else {
				mask = ((1 << (info->bits + 1)) - 1);
				value &= ~mask;
				if (info->polarity == 1)
					gain = info->max_gain - values[0];
				else
					gain =  values[0] - info->min_gain;
				value |= ((uint16)(gain / info->granularity)) & mask;
			}
			//PRINT(("B_MIX_GAIN value : %u\n", value));
			es1370_codec_write(&dev->config, info->reg, value);
			break;
		case B_MIX_MUTE:
			mask = ((1 << 1) - 1) << 15;
			value = es1370_codec_read(&dev->config, info->reg);
			value &= ~mask;
			value |= ((values[0] == 1.0 ? 1 : 0 ) << 15 & mask);
			if (info->reg == AC97_SURR_VOLUME) {
				// there is a independent mute for each channel
				mask = ((1 << 1) - 1) << 7;
				value &= ~mask;
				value |= ((values[0] == 1.0 ? 1 : 0 ) << 7 & mask);
			}
			//PRINT(("B_MIX_MUTE value : %u\n", value));
			es1370_codec_write(&dev->config, info->reg, value);
			break;
		case B_MIX_MICBOOST:
			mask = ((1 << 1) - 1) << 6;
			value = es1370_codec_read(&dev->config, info->reg);
			value &= ~mask;
			value |= ((values[0] == 1.0 ? 1 : 0 ) << 6 & mask);
			//PRINT(("B_MIX_MICBOOST value : %u\n", value));
			es1370_codec_write(&dev->config, info->reg, value);
			break;
		case B_MIX_MUX:
			mask = ((1 << 3) - 1);
			value = ((int32)values[0]) & mask;
			value = value | (value << 8);
			//PRINT(("B_MIX_MUX value : %u\n", value));
			es1370_codec_write(&dev->config, AC97_RECORD_SELECT, value);
			break;
	}

}

static int32
es1370_create_group_control(multi_dev *multi, int32 *index, int32 parent, 
	int32 string, const char* name) {
	int32 i = *index;
	(*index)++;
	multi->controls[i].mix_control.id = EMU_MULTI_CONTROL_FIRSTID + i;
	multi->controls[i].mix_control.parent = parent;
	multi->controls[i].mix_control.flags = B_MULTI_MIX_GROUP;
	multi->controls[i].mix_control.master = EMU_MULTI_CONTROL_MASTERID;
	multi->controls[i].mix_control.string = string;
	if (name)
		strcpy(multi->controls[i].mix_control.name, name);
		
	return multi->controls[i].mix_control.id;
}

static status_t
es1370_create_controls_list(multi_dev *multi)
{
	multi->control_count = 0;
	printf("multi->control_count %lu\n", multi->control_count);
	return B_OK;
}

static status_t 
es1370_get_mix(es1370_dev *card, multi_mix_value_info * mmvi)
{
	int32 i, id;
	multi_mixer_control *control = NULL;
	for (i = 0; i < mmvi->item_count; i++) {
		id = mmvi->values[i].id - EMU_MULTI_CONTROL_FIRSTID;
		if (id < 0 || id >= card->multi.control_count) {
			printf("es1370_get_mix : invalid control id requested : %li\n", id);
			continue;
		}
		control = &card->multi.controls[id];
	
		if (control->mix_control.flags & B_MULTI_MIX_GAIN) {
			if (control->get) {
				float values[2];
				control->get(card, control->cookie, control->type, values);
				if (control->mix_control.master == EMU_MULTI_CONTROL_MASTERID)
					mmvi->values[i].u.gain = values[0];
				else
					mmvi->values[i].u.gain = values[1];
			}			
		}
		
		if (control->mix_control.flags & B_MULTI_MIX_ENABLE && control->get) {
			float values[1];
			control->get(card, control->cookie, control->type, values);
			mmvi->values[i].u.enable = (values[0] == 1.0);
		}
		
		if (control->mix_control.flags & B_MULTI_MIX_MUX && control->get) {
			float values[1];
			control->get(card, control->cookie, control->type, values);
			mmvi->values[i].u.mux = (int32)values[0];
		}
	}
	return B_OK;
}

static status_t 
es1370_set_mix(es1370_dev *card, multi_mix_value_info * mmvi)
{
	int32 i, id;
	multi_mixer_control *control = NULL;
	for (i = 0; i < mmvi->item_count; i++) {
		id = mmvi->values[i].id - EMU_MULTI_CONTROL_FIRSTID;
		if (id < 0 || id >= card->multi.control_count) {
			printf("es1370_set_mix : invalid control id requested : %li\n", id);
			continue;
		}
		control = &card->multi.controls[id];
					
		if (control->mix_control.flags & B_MULTI_MIX_GAIN) {
			multi_mixer_control *control2 = NULL;
			if (i+1<mmvi->item_count) {
				id = mmvi->values[i + 1].id - EMU_MULTI_CONTROL_FIRSTID;
				if (id < 0 || id >= card->multi.control_count) {
					printf("es1370_set_mix : invalid control id requested : %li\n", id);
				} else {
					control2 = &card->multi.controls[id];
					if (control2->mix_control.master != control->mix_control.id)
						control2 = NULL;
				}
			}

			if (control->set) {
				float values[2];
				values[0] = 0.0;
				values[1] = 0.0;

				if (control->mix_control.master == EMU_MULTI_CONTROL_MASTERID)
					values[0] = mmvi->values[i].u.gain;
				else
					values[1] = mmvi->values[i].u.gain;
					
				if (control2 && control2->mix_control.master != EMU_MULTI_CONTROL_MASTERID)
					values[1] = mmvi->values[i+1].u.gain;
					
				control->set(card, control->cookie, control->type, values);
			}
			
			if (control2)
				i++;		
		}
	
		if (control->mix_control.flags & B_MULTI_MIX_ENABLE && control->set) {
			float values[1];
			
			values[0] = mmvi->values[i].u.enable ? 1.0 : 0.0;
			control->set(card, control->cookie, control->type, values);
		}
		
		if (control->mix_control.flags & B_MULTI_MIX_MUX && control->set) {
			float values[1];
			
			values[0] = (float)mmvi->values[i].u.mux;
			control->set(card, control->cookie, control->type, values);
		}
	}
	return B_OK;
}

static status_t 
es1370_list_mix_controls(es1370_dev *card, multi_mix_control_info * mmci)
{
	multi_mix_control	*mmc;
	int32 i;
	
	mmc = mmci->controls;
	if (mmci->control_count < 24)
		return B_ERROR;
			
	if (es1370_create_controls_list(&card->multi) < B_OK)
		return B_ERROR;
	for (i = 0; i < card->multi.control_count; i++) {
		mmc[i] = card->multi.controls[i].mix_control;
	}
	
	mmci->control_count = card->multi.control_count;	
	return B_OK;
}

static status_t 
es1370_list_mix_connections(es1370_dev *card, multi_mix_connection_info * data)
{
	return B_ERROR;
}

static status_t 
es1370_list_mix_channels(es1370_dev *card, multi_mix_channel_info *data)
{
	return B_ERROR;
}

/*multi_channel_info chans[] = {
{  0, B_MULTI_OUTPUT_CHANNEL, 	B_CHANNEL_LEFT | B_CHANNEL_STEREO_BUS, 0 },
{  1, B_MULTI_OUTPUT_CHANNEL, 	B_CHANNEL_RIGHT | B_CHANNEL_STEREO_BUS, 0 },
{  2, B_MULTI_OUTPUT_CHANNEL, 	B_CHANNEL_LEFT | B_CHANNEL_STEREO_BUS, 0 },
{  3, B_MULTI_OUTPUT_CHANNEL, 	B_CHANNEL_RIGHT | B_CHANNEL_STEREO_BUS, 0 },
{  4, B_MULTI_INPUT_CHANNEL, 	B_CHANNEL_LEFT | B_CHANNEL_STEREO_BUS, 0 },
{  5, B_MULTI_INPUT_CHANNEL, 	B_CHANNEL_RIGHT | B_CHANNEL_STEREO_BUS, 0 },
{  6, B_MULTI_INPUT_CHANNEL, 	B_CHANNEL_LEFT | B_CHANNEL_STEREO_BUS, 0 },
{  7, B_MULTI_INPUT_CHANNEL, 	B_CHANNEL_RIGHT | B_CHANNEL_STEREO_BUS, 0 },
{  8, B_MULTI_OUTPUT_BUS, 		B_CHANNEL_LEFT | B_CHANNEL_STEREO_BUS, 	B_CHANNEL_MINI_JACK_STEREO },
{  9, B_MULTI_OUTPUT_BUS, 		B_CHANNEL_RIGHT | B_CHANNEL_STEREO_BUS, B_CHANNEL_MINI_JACK_STEREO },
{  10, B_MULTI_INPUT_BUS, 		B_CHANNEL_LEFT | B_CHANNEL_STEREO_BUS, 	B_CHANNEL_MINI_JACK_STEREO },
{  11, B_MULTI_INPUT_BUS, 		B_CHANNEL_RIGHT | B_CHANNEL_STEREO_BUS, B_CHANNEL_MINI_JACK_STEREO },
};*/

/*multi_channel_info chans[] = {
{  0, B_MULTI_OUTPUT_CHANNEL, 	B_CHANNEL_LEFT | B_CHANNEL_STEREO_BUS, 0 },
{  1, B_MULTI_OUTPUT_CHANNEL, 	B_CHANNEL_RIGHT | B_CHANNEL_STEREO_BUS, 0 },
{  2, B_MULTI_OUTPUT_CHANNEL, 	B_CHANNEL_LEFT | B_CHANNEL_SURROUND_BUS, 0 },
{  3, B_MULTI_OUTPUT_CHANNEL, 	B_CHANNEL_RIGHT | B_CHANNEL_SURROUND_BUS, 0 },
{  4, B_MULTI_OUTPUT_CHANNEL, 	B_CHANNEL_REARLEFT | B_CHANNEL_SURROUND_BUS, 0 },
{  5, B_MULTI_OUTPUT_CHANNEL, 	B_CHANNEL_REARRIGHT | B_CHANNEL_SURROUND_BUS, 0 },
{  6, B_MULTI_INPUT_CHANNEL, 	B_CHANNEL_LEFT | B_CHANNEL_STEREO_BUS, 0 },
{  7, B_MULTI_INPUT_CHANNEL, 	B_CHANNEL_RIGHT | B_CHANNEL_STEREO_BUS, 0 },
{  8, B_MULTI_INPUT_CHANNEL, 	B_CHANNEL_LEFT | B_CHANNEL_STEREO_BUS, 0 },
{  9, B_MULTI_INPUT_CHANNEL, 	B_CHANNEL_RIGHT | B_CHANNEL_STEREO_BUS, 0 },
{  10, B_MULTI_OUTPUT_BUS, 		B_CHANNEL_LEFT | B_CHANNEL_STEREO_BUS, 	B_CHANNEL_MINI_JACK_STEREO },
{  11, B_MULTI_OUTPUT_BUS, 		B_CHANNEL_RIGHT | B_CHANNEL_STEREO_BUS, B_CHANNEL_MINI_JACK_STEREO },
{  12, B_MULTI_INPUT_BUS, 		B_CHANNEL_LEFT | B_CHANNEL_STEREO_BUS, 	B_CHANNEL_MINI_JACK_STEREO },
{  13, B_MULTI_INPUT_BUS, 		B_CHANNEL_RIGHT | B_CHANNEL_STEREO_BUS, B_CHANNEL_MINI_JACK_STEREO },
};*/


static void
es1370_create_channels_list(multi_dev *multi)
{
	es1370_stream *stream;
	uint32 index, i, mode, designations;
	multi_channel_info *chans;
	uint32 chan_designations[] = {
		B_CHANNEL_LEFT,
		B_CHANNEL_RIGHT,
		B_CHANNEL_REARLEFT,
		B_CHANNEL_REARRIGHT,
		B_CHANNEL_CENTER,
		B_CHANNEL_SUB
	};
	
	chans = multi->chans;
	index = 0;

	for (mode=ES1370_USE_PLAY; mode!=-1; 
		mode = (mode == ES1370_USE_PLAY) ? ES1370_USE_RECORD : -1) {
		LIST_FOREACH(stream, &((es1370_dev*)multi->card)->streams, next) {
			if ((stream->use & mode) == 0)
				continue;
				
			if (stream->channels == 2)
				designations = B_CHANNEL_STEREO_BUS;
			else
				designations = B_CHANNEL_SURROUND_BUS;
			
			for (i = 0; i < stream->channels; i++) {
				chans[index].channel_id = index;
				chans[index].kind = (mode == ES1370_USE_PLAY) ? B_MULTI_OUTPUT_CHANNEL : B_MULTI_INPUT_CHANNEL;
				chans[index].designations = designations | chan_designations[i];
				chans[index].connectors = 0;
				index++;
			}
		}
		
		if (mode==ES1370_USE_PLAY) {
			multi->output_channel_count = index;
		} else {
			multi->input_channel_count = index - multi->output_channel_count;
		}
	}
	
	chans[index].channel_id = index;
	chans[index].kind = B_MULTI_OUTPUT_BUS;
	chans[index].designations = B_CHANNEL_LEFT | B_CHANNEL_STEREO_BUS;
	chans[index].connectors = B_CHANNEL_MINI_JACK_STEREO;
	index++;
	
	chans[index].channel_id = index;
	chans[index].kind = B_MULTI_OUTPUT_BUS;
	chans[index].designations = B_CHANNEL_RIGHT | B_CHANNEL_STEREO_BUS;
	chans[index].connectors = B_CHANNEL_MINI_JACK_STEREO;
	index++;
	
	multi->output_bus_channel_count = index - multi->output_channel_count 
		- multi->input_channel_count;
	
	chans[index].channel_id = index;
	chans[index].kind = B_MULTI_INPUT_BUS;
	chans[index].designations = B_CHANNEL_LEFT | B_CHANNEL_STEREO_BUS;
	chans[index].connectors = B_CHANNEL_MINI_JACK_STEREO;
	index++;
	
	chans[index].channel_id = index;
	chans[index].kind = B_MULTI_INPUT_BUS;
	chans[index].designations = B_CHANNEL_RIGHT | B_CHANNEL_STEREO_BUS;
	chans[index].connectors = B_CHANNEL_MINI_JACK_STEREO;
	index++;
	
	multi->input_bus_channel_count = index - multi->output_channel_count 
		- multi->input_channel_count - multi->output_bus_channel_count;
		
	multi->aux_bus_channel_count = 0;
}


static status_t 
es1370_get_description(es1370_dev *card, multi_description *data)
{
	uint32 size;

	data->interface_version = B_CURRENT_INTERFACE_VERSION;
	data->interface_minimum = B_CURRENT_INTERFACE_VERSION;

	strncpy(data->friendly_name, FRIENDLY_NAME, 32);
	strcpy(data->vendor_info, AUTHOR);

	data->output_channel_count = card->multi.output_channel_count;
	data->input_channel_count = card->multi.input_channel_count;
	data->output_bus_channel_count = card->multi.output_bus_channel_count;
	data->input_bus_channel_count = card->multi.input_bus_channel_count;
	data->aux_bus_channel_count = card->multi.aux_bus_channel_count;
	
	size = card->multi.output_channel_count + card->multi.input_channel_count
			+ card->multi.output_bus_channel_count + card->multi.input_bus_channel_count
			+ card->multi.aux_bus_channel_count;
			
	// for each channel, starting with the first output channel, 
	// then the second, third..., followed by the first input 
	// channel, second, third, ..., followed by output bus
	// channels and input bus channels and finally auxillary channels, 

	printf("request_channel_count = %d\n",data->request_channel_count);
	if (data->request_channel_count >= size) {
		printf("copying data\n");
		memcpy(data->channels, card->multi.chans, size * sizeof(card->multi.chans[0]));
	}

	data->output_rates = B_SR_44100;// | B_SR_48000 | B_SR_CVSR;
	data->input_rates = B_SR_44100;// | B_SR_48000 | B_SR_CVSR;
	data->output_rates = B_SR_44100;
	data->input_rates = B_SR_44100;
	data->min_cvsr_rate = 0;
	data->max_cvsr_rate = 44100;

	data->output_formats = B_FMT_16BIT;
	data->input_formats = B_FMT_16BIT;
	data->lock_sources = B_MULTI_LOCK_INTERNAL;
	data->timecode_sources = 0;
	data->interface_flags = B_MULTI_INTERFACE_PLAYBACK | B_MULTI_INTERFACE_RECORD;
	data->start_latency = 3000;

	strcpy(data->control_panel,"");

	return B_OK;
}

static status_t 
es1370_get_enabled_channels(es1370_dev *card, multi_channel_enable *data)
{
	B_SET_CHANNEL(data->enable_bits, 0, true);
	B_SET_CHANNEL(data->enable_bits, 1, true);
	B_SET_CHANNEL(data->enable_bits, 2, true);
	B_SET_CHANNEL(data->enable_bits, 3, true);
	data->lock_source = B_MULTI_LOCK_INTERNAL;
/*
	uint32			lock_source;
	int32			lock_data;
	uint32			timecode_source;
	uint32 *		connectors;
*/
	return B_OK;
}

static status_t 
es1370_set_enabled_channels(es1370_dev *card, multi_channel_enable *data)
{
	printf("set_enabled_channels 0 : %s\n", B_TEST_CHANNEL(data->enable_bits, 0) ? "enabled": "disabled");
	printf("set_enabled_channels 1 : %s\n", B_TEST_CHANNEL(data->enable_bits, 1) ? "enabled": "disabled");
	printf("set_enabled_channels 2 : %s\n", B_TEST_CHANNEL(data->enable_bits, 2) ? "enabled": "disabled");
	printf("set_enabled_channels 3 : %s\n", B_TEST_CHANNEL(data->enable_bits, 3) ? "enabled": "disabled");
	return B_OK;
}

static status_t 
es1370_get_global_format(es1370_dev *card, multi_format_info *data)
{
	data->output_latency = 0;
	data->input_latency = 0;
	data->timecode_kind = 0;
	data->input.rate = B_SR_44100;
	data->input.cvsr = 44100;
	data->input.format = B_FMT_16BIT;
	data->output.rate = B_SR_44100;
	data->output.cvsr = 44100;
	data->output.format = B_FMT_16BIT;
	return B_OK;
}

static status_t 
es1370_get_buffers(es1370_dev *card, multi_buffer_list *data)
{
	uint8 i, j, pchannels, rchannels, bufcount;
	
	printf("flags = %#x\n",data->flags);
	printf("request_playback_buffers = %#x\n",data->request_playback_buffers);
	printf("request_playback_channels = %#x\n",data->request_playback_channels);
	printf("request_playback_buffer_size = %#x\n",data->request_playback_buffer_size);
	printf("request_record_buffers = %#x\n",data->request_record_buffers);
	printf("request_record_channels = %#x\n",data->request_record_channels);
	printf("request_record_buffer_size = %#x\n",data->request_record_buffer_size);
	
	pchannels = card->pstream->channels;
	rchannels = card->rstream->channels;
	
	if (data->request_playback_buffers < current_settings.buffer_count ||
		data->request_playback_channels < (pchannels) ||
		data->request_record_buffers < current_settings.buffer_count ||
		data->request_record_channels < (rchannels)) {
		printf("not enough channels/buffers\n");
	}

	data->flags = B_MULTI_BUFFER_PLAYBACK | B_MULTI_BUFFER_RECORD; // XXX ???
//	data->flags = 0;
		
	data->return_playback_buffers = current_settings.buffer_count;	/* playback_buffers[b][] */
	data->return_playback_channels = pchannels;		/* playback_buffers[][c] */
	data->return_playback_buffer_size = current_settings.buffer_frames;		/* frames */

	bufcount = current_settings.buffer_count;
	if (bufcount > data->request_playback_buffers)
		bufcount = data->request_playback_buffers;

	for (i = 0; i < bufcount; i++)
		for (j=0; j<pchannels; j++)
			es1370_stream_get_nth_buffer(card->pstream, j, i, 
				&data->playback_buffers[i][j].base,
				&data->playback_buffers[i][j].stride);
					
	data->return_record_buffers = current_settings.buffer_count;
	data->return_record_channels = rchannels;
	data->return_record_buffer_size = current_settings.buffer_frames;	/* frames */
	
	bufcount = current_settings.buffer_count;
	if (bufcount > data->request_record_buffers)
		bufcount = data->request_record_buffers;

	for (i = 0; i < bufcount; i++)
		for (j=0; j<rchannels; j++)
			es1370_stream_get_nth_buffer(card->rstream, j, i, 
				&data->record_buffers[i][j].base,
				&data->record_buffers[i][j].stride);
		
	return B_OK;
}


static void
es1370_play_inth(void* inthparams)
{
	es1370_stream *stream = (es1370_stream *)inthparams;
	//int32 count;
	
	//acquire_spinlock(&slock);
	//stream->real_time = system_time();
	stream->frames_count += current_settings.buffer_frames;
	stream->buffer_cycle = (stream->trigblk 
		+ stream->blkmod - 1) % stream->blkmod;
	stream->update_needed = true;
	//release_spinlock(&slock);
	
	//TRACE(("es1370_play_inth : cycle : %d\n", stream->buffer_cycle));
			
	//release_sem_etc(stream->card->buffer_ready_sem, 1, B_DO_NOT_RESCHEDULE);
}

static void
es1370_record_inth(void* inthparams)
{
	es1370_stream *stream = (es1370_stream *)inthparams;
	//int32 count;
	
	//acquire_spinlock(&slock);
	//stream->real_time = system_time();
	stream->frames_count += current_settings.buffer_frames;
	stream->buffer_cycle = (stream->trigblk 
		+ stream->blkmod - 1) % stream->blkmod;
	stream->update_needed = true;
	//release_spinlock(&slock);
	
	//TRACE(("es1370_record_inth : cycle : %d\n", stream->buffer_cycle));
				
	//release_sem_etc(stream->card->buffer_ready_sem, 1, B_DO_NOT_RESCHEDULE);
}

static status_t 
es1370_buffer_exchange(es1370_dev *card, multi_buffer_info *data)
{
	cpu_status status;
	es1370_stream *pstream, *rstream;
	multi_buffer_info buffer_info;

#ifdef __HAIKU__
	if (user_memcpy(&buffer_info, data, sizeof(buffer_info)) < B_OK)
		return B_BAD_ADDRESS;
#else
	memcpy(&buffer_info, data, sizeof(buffer_info));
#endif

	buffer_info.flags = B_MULTI_BUFFER_PLAYBACK | B_MULTI_BUFFER_RECORD;
	
	if (!(card->pstream->state & ES1370_STATE_STARTED))
		es1370_stream_start(card->pstream, es1370_play_inth, card->pstream);
	
	if (!(card->rstream->state & ES1370_STATE_STARTED))
		es1370_stream_start(card->rstream, es1370_record_inth, card->rstream);
	
	/*if (acquire_sem_etc(card->buffer_ready_sem, 1, B_RELATIVE_TIMEOUT | B_CAN_INTERRUPT, 100000)
		== B_TIMED_OUT) {
		printf("buffer_exchange timeout ff\n");
	}*/
	
	status = lock();
	
	LIST_FOREACH(pstream, &card->streams, next) {
		if ((pstream->use & ES1370_USE_PLAY) == 0 || 
			(pstream->state & ES1370_STATE_STARTED) == 0)
			continue;
		if (pstream->update_needed)	
			break;
	}
	
	LIST_FOREACH(rstream, &card->streams, next) {
		if ((rstream->use & ES1370_USE_RECORD) == 0 ||
			(rstream->state & ES1370_STATE_STARTED) == 0)
			continue;
		if (rstream->update_needed)	
			break;
	}
	
	if (!pstream)
		pstream = card->pstream;
	if (!rstream)
		rstream = card->rstream;
	
	/* do playback */
	buffer_info.playback_buffer_cycle = pstream->buffer_cycle;
	buffer_info.played_real_time = pstream->real_time;
	buffer_info.played_frames_count = pstream->frames_count;
	buffer_info._reserved_0 = pstream->first_channel;
	pstream->update_needed = false;
	
	/* do record */
	buffer_info.record_buffer_cycle = rstream->buffer_cycle;
	buffer_info.recorded_frames_count = rstream->frames_count;
	buffer_info.recorded_real_time = rstream->real_time;
	buffer_info._reserved_1 = rstream->first_channel;
	rstream->update_needed = false;
	unlock(status);
	
#ifdef __HAIKU__
	if (user_memcpy(data, &buffer_info, sizeof(buffer_info)) < B_OK)
		return B_BAD_ADDRESS;
#else
	memcpy(data, &buffer_info, sizeof(buffer_info));
#endif

	//TRACE(("buffer_exchange ended\n"));
	return B_OK;
}

static status_t 
es1370_buffer_force_stop(es1370_dev *card)
{
	//es1370_voice_halt(card->pvoice);
	return B_OK;
}

static status_t 
es1370_multi_control(void *cookie, uint32 op, void *data, size_t length)
{
	es1370_dev *card = (es1370_dev *)cookie;

    switch (op) {
		case B_MULTI_GET_DESCRIPTION: 
			printf("B_MULTI_GET_DESCRIPTION\n");
			return es1370_get_description(card, (multi_description *)data);
		case B_MULTI_GET_EVENT_INFO:
			printf("B_MULTI_GET_EVENT_INFO\n");
			return B_ERROR;
		case B_MULTI_SET_EVENT_INFO:
			printf("B_MULTI_SET_EVENT_INFO\n");
			return B_ERROR;
		case B_MULTI_GET_EVENT:
			printf("B_MULTI_GET_EVENT\n");
			return B_ERROR;
		case B_MULTI_GET_ENABLED_CHANNELS:
			printf("B_MULTI_GET_ENABLED_CHANNELS\n");
			return es1370_get_enabled_channels(card, (multi_channel_enable *)data);
		case B_MULTI_SET_ENABLED_CHANNELS:
			printf("B_MULTI_SET_ENABLED_CHANNELS\n");
			return es1370_set_enabled_channels(card, (multi_channel_enable *)data);
		case B_MULTI_GET_GLOBAL_FORMAT:
			printf("B_MULTI_GET_GLOBAL_FORMAT\n");
			return es1370_get_global_format(card, (multi_format_info *)data);
		case B_MULTI_SET_GLOBAL_FORMAT:
			printf("B_MULTI_SET_GLOBAL_FORMAT\n");
			return B_OK; /* XXX BUG! we *MUST* return B_OK, returning B_ERROR will prevent 
						  * BeOS to accept the format returned in B_MULTI_GET_GLOBAL_FORMAT
						  */
		case B_MULTI_GET_CHANNEL_FORMATS:
			printf("B_MULTI_GET_CHANNEL_FORMATS\n");
			return B_ERROR;
		case B_MULTI_SET_CHANNEL_FORMATS:	/* only implemented if possible */
			printf("B_MULTI_SET_CHANNEL_FORMATS\n");
			return B_ERROR;
		case B_MULTI_GET_MIX:
			printf("B_MULTI_GET_MIX\n");
			return es1370_get_mix(card, (multi_mix_value_info *)data);
		case B_MULTI_SET_MIX:
			printf("B_MULTI_SET_MIX\n");
			return es1370_set_mix(card, (multi_mix_value_info *)data);
		case B_MULTI_LIST_MIX_CHANNELS:
			printf("B_MULTI_LIST_MIX_CHANNELS\n");
			return es1370_list_mix_channels(card, (multi_mix_channel_info *)data);
		case B_MULTI_LIST_MIX_CONTROLS:
			printf("B_MULTI_LIST_MIX_CONTROLS\n");
			return es1370_list_mix_controls(card, (multi_mix_control_info *)data);
		case B_MULTI_LIST_MIX_CONNECTIONS:
			printf("B_MULTI_LIST_MIX_CONNECTIONS\n");
			return es1370_list_mix_connections(card, (multi_mix_connection_info *)data);
		case B_MULTI_GET_BUFFERS:			/* Fill out the struct for the first time; doesn't start anything. */
			printf("B_MULTI_GET_BUFFERS\n");
			return es1370_get_buffers(card, data);
		case B_MULTI_SET_BUFFERS:			/* Set what buffers to use, if the driver supports soft buffers. */
			printf("B_MULTI_SET_BUFFERS\n");
			return B_ERROR; /* we do not support soft buffers */
		case B_MULTI_SET_START_TIME:			/* When to actually start */
			printf("B_MULTI_SET_START_TIME\n");
			return B_ERROR;
		case B_MULTI_BUFFER_EXCHANGE:		/* stop and go are derived from this being called */
			//TRACE(("B_MULTI_BUFFER_EXCHANGE\n"));
			return es1370_buffer_exchange(card, (multi_buffer_info *)data);
		case B_MULTI_BUFFER_FORCE_STOP:		/* force stop of playback, nothing in data */
			printf("B_MULTI_BUFFER_FORCE_STOP\n");
			return es1370_buffer_force_stop(card);
	}
	printf("ERROR: unknown multi_control %#x\n", op);
	return B_ERROR;
}

status_t es1370_open(const char *name, uint32 flags, void** cookie);
static status_t es1370_close(void* cookie);
static status_t es1370_free(void* cookie);
static status_t es1370_control(void* cookie, uint32 op, void* arg, size_t len);
static status_t es1370_read(void* cookie, off_t position, void *buf, size_t* num_bytes);
static status_t es1370_write(void* cookie, off_t position, const void* buffer, size_t* num_bytes);

device_hooks multi_hooks = {
	es1370_open, 			/* -> open entry point */
	es1370_close, 			/* -> close entry point */
	es1370_free,			/* -> free cookie */
	es1370_control, 		/* -> control entry point */
	es1370_read,			/* -> read entry point */
	es1370_write,			/* -> write entry point */
	NULL,					/* start select */
	NULL,					/* stop select */
	NULL,					/* scatter-gather read from the device */
	NULL					/* scatter-gather write to the device */
};


extern es1370_dev* g_card;
 status_t
es1370_open(const char *name, uint32 flags, void** cookie)
{
	es1370_dev *card = NULL;
	void *settings_handle = NULL;
	int ix;
	
	printf("open()\n");
	
	for (ix=0; ix<num_cards; ix++) {
		if (!strcmp(cards[ix].name, name)) {
			card = &cards[ix];
		}
	}
	card = g_card;

	
	if (card == NULL) {
		printf("open() card not found %s\n", name);
		for (ix=0; ix<num_cards; ix++) {
			printf("open() card available %s\n", cards[ix].name);
		}
		return B_ERROR;
	}
		
	printf("open() got card\n");
	
	if (card->pstream !=NULL)
		return B_ERROR;
	if (card->rstream !=NULL)
		return B_ERROR;
			
	*cookie = card;
	card->multi.card = card;
		
	// get driver settings
	//settings_handle = load_driver_settings(ES1370_SETTINGS);
	if (settings_handle != NULL) {
		const char *item;
		char       *end;
		uint32      value;

		//item = get_driver_parameter (settings_handle, "sample_rate", "44100", "44100");
		value = strtoul (item, &end, 0);
		if (*end == '\0') 
			current_settings.sample_rate = value;

		//item = get_driver_parameter (settings_handle, "buffer_frames", "512", "512");
		value = strtoul (item, &end, 0);
		if (*end == '\0') 
			current_settings.buffer_frames = value;

		//item = get_driver_parameter (settings_handle, "buffer_count", "2", "2");
		value = strtoul (item, &end, 0);
		if (*end == '\0') 
			current_settings.buffer_count = value;

		//unload_driver_settings(settings_handle);
	}

	printf("stream_new\n");
		
	card->rstream = es1370_stream_new(card, ES1370_USE_RECORD, current_settings.buffer_frames, current_settings.buffer_count);
	card->pstream = es1370_stream_new(card, ES1370_USE_PLAY, current_settings.buffer_frames, current_settings.buffer_count);
	
	//card->buffer_ready_sem = create_sem(0, "pbuffer ready");
		
	printf("stream_setaudio\n");
	
	es1370_stream_set_audioparms(card->pstream, 2, true, current_settings.sample_rate);
	es1370_stream_set_audioparms(card->rstream, 2, true, current_settings.sample_rate);
		
	card->pstream->first_channel = 0;
	card->rstream->first_channel = 2;
	
	es1370_stream_commit_parms(card->pstream);
	es1370_stream_commit_parms(card->rstream);
	
	es1370_create_channels_list(&card->multi);
		
	return B_OK;
}

static status_t
es1370_close(void* cookie)
{
	//es1370_dev *card = cookie;
	printf("close()\n");
		
	return B_OK;
}

static status_t
es1370_free(void* cookie)
{
	es1370_dev *card = cookie;
	es1370_stream *stream;
	printf("free()\n");
		
	if (card->buffer_ready_sem > B_OK)
		;
		//delete_sem(card->buffer_ready_sem);
			
	LIST_FOREACH(stream, &card->streams, next) {
		es1370_stream_halt(stream);
	}
	
	while (!LIST_EMPTY(&card->streams)) {
		es1370_stream_delete(LIST_FIRST(&card->streams));
	}

	card->pstream = NULL;
	card->rstream = NULL;
	
	return B_OK;
}

static status_t
es1370_control(void* cookie, uint32 op, void* arg, size_t len)
{
	return es1370_multi_control(cookie, op, arg, len);
}

static status_t
es1370_read(void* cookie, off_t position, void *buf, size_t* num_bytes)
{
	*num_bytes = 0;				/* tell caller nothing was read */
	return B_IO_ERROR;
}

static status_t
es1370_write(void* cookie, off_t position, const void* buffer, size_t* num_bytes)
{
	*num_bytes = 0;				/* tell caller nothing was written */
	return B_IO_ERROR;
}

