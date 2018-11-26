#define SB_C
#include "toolkit.h"
#include "string.h"
#include "stdio.h"
#include "fileio.h"
#include "getenv.h"
#include "memory.h"
#include "ctype.h"

extern u32int malloc(u32int sz);

BINT8                ERRORSTRING[04][150] = { "The BLASTER environment string is not set",
"An error occured attempting to reset the SoundBlaster",
"The sample rate is too fast for this SoundBlaster",
"This SoundBlaster only supports mono playback" };
BINT16               SB_IntCount = 0;

BINT16 SB_GetEnv(VBINT)
{
	BINT8   *envstr;
	BINT8   *tmpstr;

	tmpstr = getenv("BLASTER");
	if (tmpstr == NULL)
		err(1);

	envstr = strdup(tmpstr);

	tmpstr = strtok(tmpstr, " \t");

	while (tmpstr)
	{
		switch (toupper(tmpstr[0]))
		{
		case 'A':
			SB_BaseAddr = (BINT16)strtol(tmpstr + 1, NULL, 16);
			break;
		case 'I':
			SB_IrqNum = (BINT16)atoi(tmpstr + 1);
			break;
		case 'D':
			SB_LowDma = (BINT16)atoi(tmpstr + 1);
			break;
		case 'H':
			SB_HighDma = (BINT16)atoi(tmpstr + 1);
			break;
		case 'P':
			MIDI_BaseAddr = (BINT16)strtol(tmpstr + 1, NULL, 16);
			break;
		case 'T':
			SB_Type = (BINT16)atoi(tmpstr + 1);
			break;
		case 'M':
			MIXER_BaseAddr = (BINT16)strtol(tmpstr + 1, NULL, 16);
			break;
		}
		tmpstr = strtok(NULL, " \t");      //find next token
	}
	return(1);
}

BINT16 SB_Init()
{
	BINT16 x;

	if (!SB_GetEnv())
		return(0);
	if (!DSP_Reset())
		return(0);
	DSP_GetVersion();

	SB_Auto = ((DSP_VERMAJ > 2) || ((DSP_VERMAJ == 2) && (DSP_VERMIN >= 1)));
	SB16 = (DSP_VERMAJ >= 4);
	if (SB_Auto)
		DMA_LEN = DMA_BUFFER;
	else
		DMA_LEN = DMA_BLOCK;

	if (SB_IrqNum < 8)
	{ /* PIC1 */
		IRQ_VECTOR = 0x08 + SB_IrqNum;
		PIC_ROTATEPORT = 0x20;
		PIC_MASKPORT = 0x21;
	}
	else
	{ /* PIC2 */
		IRQ_VECTOR = 0x70 + SB_IrqNum - 8;
		PIC_ROTATEPORT = 0xA0;
		PIC_MASKPORT = 0xA1;
	}

	IRQ_STOPMASK = 1 << (SB_IrqNum % 8);
	IRQ_STARTMASK = ~IRQ_STOPMASK;

	SB_AllocBuf();

	SB_SetISR();

	for (x = 0; x < NUMSOUNDS; x++)
	{
		SB_SOUND_INUSE[x] = 0;
		SB_SOUNDS[x].loop = 0;
	}


	return(1);
}

static NOBINT16 sr = 0, nc = MONO, b16 = 0;

BINT16 SB_PlaySound(SOUND_PTR s, BINT16 loop)
{
	BINT16 x;

	s->curpos = 0;
	s->bytesleft = s->length;
	s->loop = loop;

	if ((s->rate != sr) || (nc != s->nc) || (b16 != s->b16))
	{
		if (SB_IntCount)
			DSP_StopDAC(b16);

		SB_IntCount = 0;

		memcpy(&SB_SOUNDS[0], s, sizeof(SOUND));
		b16 = s->b16;
		sr = s->rate;
		nc = s->nc;

		if (s->b16)
			SB_ACKPORT = 0x0F;
		else
			SB_ACKPORT = 0x0E;

		DMA_Init(SB_LowDma, SB_HighDma, SB_Auto, s->b16);

		if (s->b16)
			memset((BINT8 *)SB_Buf16, 0x00, DMA_BUFFER * 2);
		else
			memset((BINT8 *)SB_Buf8, 0x80, DMA_BUFFER);

		DSP_StartDAC(s->rate, s->nc, s->b16);
	}

	for (x = 0; x < NUMSOUNDS; x++)
		if (!SB_SOUND_INUSE[x])
		{
			SB_SOUND_INUSE[x] = 1;
			memcpy(&SB_SOUNDS[x], s, sizeof(SOUND));
			return(x);
		}
	return(x);
}

VBINT SB_Done(VBINT)
{
	DSP_StopDAC(0);
	SB_ResetISR();
	SB_FreeBuf();
}

VBINT SB_MixSounds8(VBINT)
{
	BINT16 i, x, off;
	BINT16 tmp;

	off = (SB_IntCount & 1)*DMA_BLOCK;
	for (x = 0; x < DMA_BLOCK; x++)
	{
		tmp = 0x80;
		for (i = 0; i < NUMSOUNDS; i++)
			if (SB_SOUND_INUSE[i])
			{
				if (!(SB_SOUNDS[i].bytesleft--))
				{
					SB_SOUND_INUSE[i] = 0;
				}
				tmp += (BINT16)SB_SOUNDS[i].hSnd8[SB_SOUNDS[i].curpos + x];
			}
		if (tmp > 0xff)
			tmp = 0xff;
		else if (tmp < 0)
			tmp = 0;

		SB_Buf8[off + x] = (BINT8)tmp;
	}
	for (i = 0; i < NUMSOUNDS; i++)
		if (SB_SOUND_INUSE[i])
			SB_SOUNDS[i].curpos += DMA_BLOCK;
}

VBINT SB_MixSounds16(VBINT)
{
	BINT16 i, x, off;
	BINT32 tmp;
	off = (SB_IntCount & 1)*DMA_BLOCK;

	for (x = 0; x < DMA_BLOCK; x++)
	{
		tmp = 0;
		for (i = 0; i < NUMSOUNDS; i++)
			if (SB_SOUND_INUSE[i])
			{
				SB_SOUNDS[i].bytesleft -= 2;
				if (SB_SOUNDS[i].bytesleft <= 0)
				{
					SB_SOUND_INUSE[i] = 0;
				}
				tmp += SB_SOUNDS[i].hSnd16[SB_SOUNDS[i].curpos + x];
			}
		if (tmp > 32768)
			tmp = 32738;
		else if (tmp < -32767)
			tmp = -32767;

		SB_Buf16[off + x] = tmp;
	}
	for (i = 0; i < NUMSOUNDS; i++)
		if (SB_SOUND_INUSE[i])
			SB_SOUNDS[i].curpos += DMA_BLOCK;
}

//VBINT _interrupt _loadds _saveregs SB_SoundIsr(VBINT)
VBINT SB_SoundIsr(VBINT)
{
	BINT32 t;

	if (!SB_Auto)
	{
		t = (BINT32)SB_Buf8;
		DMA_GoSc(t >> 16, (t % 65536) + ((SB_IntCount + 1) & 1)*DMA_BLOCK);
		DSP_Write(BIT8_SINGLE);
		DSP_Write(lo(DMA_BLOCK - 1));
		DSP_Write(hi(DMA_BLOCK - 1));
	}

	if (b16)
		SB_MixSounds16();
	else
		SB_MixSounds8();

	for (t = 0; t < NUMSOUNDS; t++)
	{
		if (SB_SOUNDS[t].bytesleft <= 0)
		{
			if (SB_SOUNDS[t].loop)
			{
				SB_SOUNDS[t].curpos = 0;
				SB_SOUNDS[t].bytesleft = SB_SOUNDS[t].length;
				SB_SOUND_INUSE[t] = 1;
			}
		}
	}
	SB_IntCount++;

	inp(SB_BaseAddr + SB_ACKPORT);       /* Acknowledge interrupt with sound card */
	outp(0xA0, 0x20);   /* Acknowledge interrupt with PIC2 */
	outp(0x20, 0x20);   /* Acknowledge interrupt with PIC1 */
}

VBINT SB_SetISR()
{
	NOBINT8 x;
	_disable();

	x = inp(PIC_MASKPORT);
	x |= IRQ_STOPMASK;
	outp(PIC_MASKPORT, x);

	SB_OldIsr = _dos_getvect(IRQ_VECTOR);
	_dos_setvect(IRQ_VECTOR, SB_SoundIsr);

	x = inp(PIC_MASKPORT);
	x &= IRQ_STARTMASK;

	outp(PIC_MASKPORT, x);

	_enable();
}

VBINT SB_ResetISR()
{
	_disable();

	outp(PIC_MASKPORT, (inp(PIC_MASKPORT) | IRQ_STOPMASK));

	_dos_setvect(IRQ_VECTOR, SB_OldIsr);

	_enable();
}

VBINT SB_AllocBuf(VBINT)
{

	/* We allocate a selector in this way for two reasons:
	   1. DMA can only access real mode DOS memory.
	   2. I said moose.
	*/

	DPMI_AllocCM((DMA_BUFFER * 2) / 16, (BINT16 *)&SB_BufSel, (BINT16 *)&SB_BufSeg);
	SB_Buf8 = (NOBINT8 *)(SB_BufSeg << 4);
	SB_Buf16 = (BINT16 *)(SB_Buf8);
}

VBINT SB_FreeBuf(VBINT)
{
	DPMI_FreeCM(SB_BufSel);
}

BINT16 SB_LoadWave(BINT8 *fn, SOUND_PTR w)
{
	STR      str[200];
	FILE     *fp;
	NOBINT32 rID, x;
	NOBINT32 rLen;
	NOBINT32 wID;
	NOBINT32 fID;
	NOBINT32 fLen;
	NOBINT32 fNext;
	NOBINT16 fTag;
	NOBINT16 dummy;
	NOBINT16 wFormatTag;
	NOBINT16 nChannels;
	NOBINT16 nSamplesPerSec;
	NOBINT32 dID;
	NOBINT32 dLen;

	if ((fp = fopen(fn, "rb")) == NULL)
	{
		strcpy(str, fn);
		strcat(str, ".wav");
		if ((fp = fopen(str, "rb")) == NULL)
			return(0);
	}
	fread(&rID, 1, 4, fp);

	if (rID != 0x46464952)
		return(0);

	fread(&rLen, 1, 4, fp);
	fread(&wID, 1, 4, fp);

	if (wID != 0x45564157)
		return(0);

	fread(&fID, 1, 4, fp);
	if (fID != 0x20746D66)
		return(0);

	fread(&fLen, 1, 4, fp);
	fNext = fLen + ftell(fp);
	fread(&wFormatTag, 1, 2, fp);
	if (wFormatTag != 1)
		return(0);

	fread(&nChannels, 1, 2, fp);
	w->nc = nChannels;

	fread(&nSamplesPerSec, 1, 2, fp);
	w->rate = nSamplesPerSec;

	for (x = 0; x < 8; x++)
	{
		dummy = 0;
		fread(&dummy, 1, 1, fp);
	}
	fTag = 0;
	fread(&fTag, 1, 1, fp);

	w->b16 = (fTag == 16);

	fseek(fp, fNext, 0);
	fread(&dID, 1, 4, fp);
	if (dID != 0x61746164)
		return(0);

	fread(&dLen, 1, 4, fp);
	w->length = dLen;

	if ((w->hSnd8 = (BINT8 *)malloc(dLen)) == NULL)
		return(0);

	w->hSnd16 = (BINT16 *)w->hSnd8;
	fread(w->hSnd8, 1, dLen, fp);
	fclose(fp);

	if (!w->b16)
		for (x = 0; x < dLen; x++)
			w->hSnd8[x] += 0x80;

	return(1);
}

