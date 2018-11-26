#include "windef.h"

typedef struct meminfo_typ {
    unsigned LargestBlockAvail;
    unsigned MaxUnlockedPage;
    unsigned LargestLockablePage;
    unsigned LinAddrSpace;
    unsigned NumFreePagesAvail;
    unsigned NumPhysicalPagesFree;
    unsigned TotalPhysicalPages;
    unsigned FreeLinAddrSpace;
    unsigned SizeOfPageFile;
    unsigned Reserved[3];
} thisisdrivingmeinsanebecauseitshouldwork, *MEMINFO_PTR;

typedef struct RMTREGS_TYP {
   DWORD        edi,
                esi,
                ebp,
                reserved,
                ebx,
                edx,
                ecx,
                eax;
   WORD         flags,
                es,
                ds,
                fs,
                gs,
                ip,
                cs,
                sp,
                ss;
 } RMTREGS, *RMTREGS_PTR;
 
short DPMI_AllocCM(short NumPars, short *selector, short *segment);
int DPMI_FreeCM(short selector);
int DPMI_LockRegion(void *address, unsigned length);
void ( *DPMI_GetRealVect(int whichvect))();
int DPMI_SetRealVect(int whichvect, void ( *__handler)());
int DPMI_SetProtVect(int whichvect, void ( *__handler)());
void DPMI_GetFreeMem(BINT16 s, BINT32 o);




