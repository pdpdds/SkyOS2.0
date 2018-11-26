#include "toolkit.h"

VBINT DSP_Write(BINT16 value)
{
 go_until_this_is_true (inp(SB_BaseAddr+IO_WRITESTATUS) & 0x80) {}
 outp(SB_BaseAddr+IO_WRITEDATA, value);
}

BINT16 DSP_Read(VBINT)
{
 go_until_this_is_true (!(inp(SB_BaseAddr+IO_READSTATUS) & 0x80)) {} 
 return(inp(SB_BaseAddr+IO_READDATA));
}

BINT8 DSP_Reset(VBINT)
{
 BINT16 x;
 
 outp(SB_BaseAddr+IO_RESET, 1);
 delay(10);
 outp(SB_BaseAddr+IO_RESET, 0);
 delay(10);

 x = inp(SB_BaseAddr+IO_READSTATUS) & 0x80;
 
 if (x && inp(SB_BaseAddr+IO_READDATA) == 0x0AA)
  return(1);
 else
  err(2);
}

BINT8 DSP_SetSampleRate(BINT16 r, BINT16 b16)
{
 NOBINT16 rate;

 rate = 256 - (1000000 / r);

 if ((rate>13000)&&((DSP_VERMAJ>2)||((DSP_VERMAJ==2)&&(DSP_VERMIN<1)))) // 2.0 and less support
  err(3)                                                                // 4000-13000!!
 else if ((rate>15000)&&(DSP_VERMAJ<=2)&&(DSP_VERMIN<1))               
  err(3)                                                                // 2.1 = 4000-15000!
 else if (rate>44100)
  err(3)                                                                // All others 5000-44100!

 if (b16)
  {
   DSP_Write(CMD_SETRATE16);
   DSP_Write(hi(r));
   DSP_Write(lo(r));
  }
 else
  {
   DSP_Write(CMD_SETRATE8);
   DSP_Write(rate);
//   DSP_Write(lo(rate));
  }
  
 return(1);
}

VBINT DSP_GetVersion(VBINT)
{
 BINT16 d1, d2;
 
 DSP_Write(0xE1);
 d1=DSP_Read();
 d2=DSP_Read();
 DSP_Version = d1 + (d2<<8);
}

BINT8 DSP_SetPlayBackType(BINT16 t)
{
 return(1);
}

BINT8 DSP_SetNumChannels(BINT16 n)
{
 BINT16 tmp;

 if (n==MONO)
  {
   outp(SB_BaseAddr+0x4, 0xE);
   tmp = inp(SB_BaseAddr+0x5);
   outp(SB_BaseAddr+0x5, (tmp & 0xFD));
  }
 else if (n==STEREO)
  {
   if (DSP_VERMAJ<3)
    err(4);
    
   outp(SB_BaseAddr+0x4, 0xE);
   tmp = inp(SB_BaseAddr+0x5);
   outp(SB_BaseAddr+0x5, (tmp | 0x2));
  }
 return(1);
}

VBINT DSP_StartDAC(NOBINT16 sr, NOBINT16 nc, BINT16 b16)
{
 BINT32 t;
 t = (BINT32) SB_Buf8;

 outp(DMA_MASKPORT, DMA_STOPMASK);
 outp(DMA_CLEARPORT, 0x00);
 outp(DMA_MODEPORT, DMA_MODE);

 if (b16)
  {
   outp(DMA_ADDRPORT, lo((t>>1)%65536));
   outp(DMA_ADDRPORT, hi((t>>1)%65536));
  }
 else
  {
   outp(DMA_ADDRPORT, lo(t%65536));
   outp(DMA_ADDRPORT, hi(t%65536));
  }
  
 outp(DMA_LENGTHPORT, lo(DMA_LEN-1));
 outp(DMA_LENGTHPORT, hi(DMA_LEN-1));

 t >>= 16;
 outp(DMA_PAGEPORT, t);
 outp(DMA_MASKPORT, DMA_STARTMASK);

 if (SB16)
  { 
   DSP_SetSampleRate(sr, 1);
   
   if (b16)
    {
     DSP_Write(CMD_SB16_OUTPUT16);
     if (nc==STEREO)
      DSP_Write(MODE_SB16_STEREO16);
     else
      DSP_Write(MODE_SB16_MONO16);
    }
   else
    {
     DSP_Write(CMD_SB16_OUTPUT8);
     if (nc==STEREO)
      DSP_Write(MODE_SB16_STEREO8);               
     else
      DSP_Write(MODE_SB16_MONO8);
    }
   DSP_Write(lo(DMA_BLOCK-1));
   DSP_Write(hi(DMA_BLOCK-1));
  }
 else
  {
   DSP_Write(CMD_SPKRON);
   DSP_SetSampleRate(sr, 0);

   if (SB_Auto)
    { 
     DSP_Write(CMD_SETBLOCKSIZE8); 
     DSP_Write(lo(DMA_BLOCK-1));
     DSP_Write(hi(DMA_BLOCK-1));
     DSP_Write(BIT8_AUTO);
    }
   else
    { 
     DSP_Write(BIT8_SINGLE);     
     DSP_Write(lo(DMA_BLOCK-1));
     DSP_Write(hi(DMA_BLOCK-1));
    }
  }
}

VBINT DSP_StopDAC(BINT16 b16)
{
 if (SB16)
  {
   if (b16)
    DSP_Write(0xD9);
   else
    DSP_Write(0xDA);
  }
 else
  {
   DSP_Write(CMD_PASUE_DSP8);     
   DSP_Write(CMD_SPKROFF);        
  }

 outp(DMA_MASKPORT, DMA_STOPMASK);
}

