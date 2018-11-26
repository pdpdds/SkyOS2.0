#include "tools.h"

#define NUMSOUNDS                       16

#define IO_RESET                        0x6
#define IO_READDATA                     0xA
#define IO_WRITEDATA                    0xC
#define IO_WRITESTATUS                  0xC
#define IO_READSTATUS                   0xE

#define CMD_SETRATE8                    0x40
#define CMD_SETRATE16                   0x41
#define CMD_SETBLOCKSIZE8               0x48
#define CMD_SPKRON                      0xD1
#define CMD_SPKROFF                     0xD3
#define CMD_SB16_OUTPUT8                0xC6
#define CMD_SB16_OUTPUT16               0xB6
#define CMD_PAUSE_DSP16                 0xD5
#define CMD_PASUE_DSP8                  0xD0

#define MODE_SB16_MONO16                0x10
#define MODE_SB16_STEREO16              0x30
#define MODE_SB16_MONO8                 0x00
#define MODE_SB16_STEREO8               0x20

#define MONO                            1
#define STEREO                          2

#define BIT8_SINGLE                     0x14
#define BIT8_AUTO                       0x1C
#define BIT8_SINGLE_HIGH                0x91
#define BIT8_AUTO_HIGH                  0x90

#define err(n)                          {SB_Status=n; return(0);}
#define ERROR_NOENVSTRING               1
#define ERROR_FAILRESET                 2
#define ERROR_BADRATE                   3
#define ERROR_BADCHANNELS               4

#define DSP_VERMAJ                      (DSP_Version & 0xFF)
#define DSP_VERMIN                      (DSP_Version >> 8)

#define DMA_BUFFER                      256
#define DMA_BLOCK                       (DMA_BUFFER / 2)

#define MIX_RESET		        0
#define MIX_MASTER			1
#define MIX_CD				2
#define MIX_VOICE			3

#define CT1335_MASTER                   0x02
#define CT1335_MIDI                     0x06
#define CT1335_CD                       0x08
#define CT1335_VOICE                    0x0A

#define CT1345_MASTER                   0x22
#define CT1345_VOICE                    0x04
#define CT1345_MIDI                     0x26
#define CT1345_CD                       0x28
#define CT1345_LINE                     0x2E

#define CT1745_MASTER                   0x30
#define CT1745_VOICE                    0x32
#define CT1745_MIDI                     0x34
#define CT1745_CD                       0x36
#define CT1745_LINE                     0x38

typedef struct SOUND_TYP
{
 BINT8          b16, loop;
 BINT8          nc;
 NOBINT16       rate;
 BINT8          *hSnd8;
 BINT16         *hSnd16;
 BINT32         curpos;
 BINT32         length;
 BINT32         bytesleft;
} SOUND, *SOUND_PTR;

BINT16  SB_Init(VBINT);
VBINT   SB_Done(VBINT);
BINT16  SB_GetEnv(VBINT);
VBINT   SB_SetISR(VBINT);
VBINT   SB_ResetISR(VBINT);
VBINT   SB_AllocBuf(VBINT);
VBINT   SB_FreeBuf(VBINT);
BINT16  SB_LoadWave(STR *fn, SOUND_PTR w);
BINT16  SB_PlaySound(SOUND_PTR s, BINT16 loop);
VBINT   SB_MixSounds8(VBINT);
VBINT   SB_MixSounds16(VBINT);
BINT8   DSP_Reset(VBINT);
VBINT   DSP_Write(BINT16 value);
BINT16  DSP_Read(VBINT);
BINT8   DSP_SetSampleRate(BINT16 r, BINT16 b16);
BINT8   DSP_SetPlayBackType(BINT16 t);
VBINT   DSP_GetVersion(VBINT);
VBINT   DSP_StartDAC(NOBINT16 sr, NOBINT16 nc, BINT16 b16);
VBINT   DSP_StopDAC(BINT16 b16);

VBINT   DMA_Init(BINT16 dma8, BINT16 dma16, BINT16 autoinit, BINT8 bit16);
VBINT   DMA_GoSc(BINT16 seg, BINT16 ofs);

BINT16  MIXER_Init(VBINT);
VBINT   MIXER_WriteVolume(BINT8 reg, BINT8 val);
NOBINT8 MIXER_ReadVolume(BINT8 reg);
VBINT   MIXER_SetVolume(BINT8 reg,  BINT8 lev);
NOBINT8 MIXER_GetVolume(BINT8 reg);

//#ifndef SB_C
 //#define e extern
//#else
 #define e
 /*BINT8                ERRORSTRING[04][150] = {"The BLASTER environment string is not set",
                                              "An error occured attempting to reset the SoundBlaster",
                                              "The sample rate is too fast for this SoundBlaster",
                                              "This SoundBlaster only supports mono playback"};*/
 //BINT16               SB_IntCount=0;
//#endif

extern BINT16 SB_IntCount;
 e BINT16               SB_BaseAddr,
                        SB_IrqNum,
                        SB_LowDma,
                        SB_HighDma,
                        SB_Type,
                        SB_Status,                        
                        SB_ACKPORT,
                        MIXER_BaseAddr,                       
                        MIDI_BaseAddr;
 e BINT16               DSP_Version;
 e BINT16               SB_Auto, SB16;
 //e VBINT                (_interrupt * SB_OldIsr)();
 e VBINT				(* SB_OldIsr)();
 extern BINT8                ERRORSTRING[04][150];
 e BINT16               IRQ_VECTOR,
                        PIC_MASKPORT,
                        PIC_ROTATEPORT;
 e NOBINT8              IRQ_STOPMASK,
                        IRQ_STARTMASK;
 e BINT16               SB_BufSel;
 e BINT16               SB_BufSeg;
 e BINT16               *SB_Buf16;
 e NOBINT8              *SB_Buf8, *pb;

 e SOUND                SB_SOUNDS[NUMSOUNDS];
 e NOBINT16             SB_SOUND_INUSE[NUMSOUNDS];
 e NOBINT16             DMA_PAGEPORT,
                        DMA_LENGTHPORT,
                        DMA_MASKPORT,
                        DMA_CLEARPORT,
                        DMA_ADDRPORT,
                        DMA_MODEPORT,
                        DMA_STOPMASK,
                        DMA_STARTMASK,
                        DMA_MODE,
                        DMA_LEN;
#undef e                                                

