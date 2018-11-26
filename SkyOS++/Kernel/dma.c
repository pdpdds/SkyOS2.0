#include "windef.h"

#include "toolkit.h"

VBINT DMA_Init(BINT16 dma8, BINT16 dma16, BINT16 autoinit, BINT8 bit16)
{

 if (bit16)
  {
   DMA_MASKPORT = 0xD4;
   DMA_CLEARPORT = 0xD8;
   DMA_MODEPORT = 0xD6;
   DMA_ADDRPORT = 0xC0 + 4*(dma16-4);
   DMA_LENGTHPORT = 0xC2 + 4*(dma16-4);

   switch (dma16)
    {
     case 5:
      DMA_PAGEPORT = 0x8B;
      break;
     case 6:
      DMA_PAGEPORT = 0x89;
      break;
     case 7:
      DMA_PAGEPORT = 0x8A;
      break;
    }

   DMA_STOPMASK = dma16-4 + 0x04;   /* 000001xx */
   DMA_STARTMASK = dma16-4 + 0x00;  /* 000000xx */

   if (autoinit)
    DMA_MODE = dma16-4 + 0x58;     /* 010110xx */
   else
    DMA_MODE = dma16-4 + 0x48;     /* 010010xx */
  }
 else
  {
   DMA_MASKPORT = 0x0A;
   DMA_CLEARPORT = 0x0C;
   DMA_MODEPORT = 0x0B;
   DMA_ADDRPORT = 0x00 + 2*dma8;
   DMA_LENGTHPORT = 0x01 + 2*dma8;

   switch (dma8)
    {
     case 0:
      DMA_PAGEPORT = 0x87;
      break;
     case 1:
      DMA_PAGEPORT = 0x83;
      break;
     case 2:
      DMA_PAGEPORT = 0x81;
      break;
     case 3:
      DMA_PAGEPORT = 0x82;
      break;
    }

   DMA_STOPMASK = dma8 + 0x04;       /* 000001xx */
   DMA_STARTMASK = dma8 + 0x00;      /* 000000xx */

   if (autoinit)
    DMA_MODE = dma8 + 0x58;      /* 010110xx */
   else
    DMA_MODE = dma8 + 0x48;      /* 010010xx */
  }
}

VBINT DMA_GoSc(BINT16 seg, BINT16 ofs) 
{
 outp(DMA_MASKPORT, DMA_STOPMASK);
 outp(DMA_CLEARPORT, 0x00);
 outp(DMA_MODEPORT, DMA_MODE);
 outp(DMA_ADDRPORT, lo(ofs));
 outp(DMA_ADDRPORT, hi(ofs));
 outp(DMA_LENGTHPORT, lo(DMA_BLOCK-1));
 outp(DMA_LENGTHPORT, hi(DMA_BLOCK-1));
 outp(DMA_PAGEPORT, seg);
 outp(DMA_MASKPORT, DMA_STARTMASK);
}

