#include "windef.h"

#ifndef Hi
  #define Hi(n) ((n>>8) & 0x00FF)
  #define hi(n) ((n>>8) & 0x00FF)
#endif

#ifndef Lo
  #define Lo(n) (n & 0x00FF)
  #define lo(n) (n & 0x00FF)
#endif

#ifndef Pointer
  #define Pointer unsigned char far *
#endif

#ifndef BYTE
        #define BYTE unsigned char
#endif

#ifndef WORD
        #define WORD unsigned short
#endif

#ifndef DWORD
        #define DWORD unsigned long
#endif

#ifndef INT8
        #define INT8 signed char
#endif

#ifndef INT16
        #define INT16 signed short
#endif

#ifndef INT32
        #define INT32 signed long
#endif

#ifndef forever
        #define forever for (dummy=0;dummy<1;dummy=0)  
#endif

#ifndef MX
  #define MX      (MOUSE_X)
  #define MY      (MOUSE_Y)
  #define ML      (MOUSE_BUTTON_LEFT)
  #define MR      (MOUSE_BUTTON_RIGHT)
#endif

#ifndef MAX_FILES
  #define MAX_FILES       100
  #define PATH_LEN        128   
  #define NAME_LEN        13    
#endif

#ifndef STUPID_MACROS
  #define STUPID_MACROS
  #define begin                 }
  #define end                   {
  #define go_until_this_is_over for
  #define go_until_this_is_true while
  #define dont                  do
  #define enter                 return
  #define noneggy               unsigned
  #define neggy                 signed
  #define ONION                 union
  #define RINGS                 REGS
  #define STRUT                 struct
  #define NOBINT                unsigned short int
  #define NOBINT8               noneggy char
  #define NOBINT16              noneggy short int
  #define NOBINT32              noneggy long
  #define BINT                  signed short int
  #define BINT8                 neggy char
  #define BINT16                neggy short int
  #define BINT32                neggy long
  #define STR                   char
  #define FLOTE                 float
  #define VBINT                 void
  #define then                  if
  #define also                  else
  #define REAL_SEG(__p) ((unsigned)((unsigned long)(void *)(__p) >> 16))
  #define REAL_OFF(__p) ((unsigned short int)__p)
#endif

#ifdef TOOLS_C
 BINT8 FileList [MAX_FILES][NAME_LEN];
 BINT8 FilePath [PATH_LEN];
 BINT  dummy;
 NOBINT16 drives[27]= {0};   // a to z, don't use first char!!
#else
 extern BINT8    FileList [MAX_FILES][NAME_LEN];
 extern BINT8    FilePath [PATH_LEN];
 extern BINT     dummy;
 extern NOBINT16 drives[27];   // a to z, don't use first char!!
#endif

int  SearchFile(char *szCmdArg, char *endd);
void GetAvailableDrives(void);

#define _interrupt 
