#include "SystemAPI.h"
#include "intrinsic.h"
#include "PEImage.h"
#include "sprintf.h"
#include "SkyConsole.h"
#include "PlatformAPI.h"
#include "string.h"

DWORD g_dwSysLastError = 0;

void __SKY_ASSERT(const char* expr_str, bool expr, const char* file, int line, const char* msg)
{
	if (!expr)
	{
		//SkyConsole::Print("%s %s, %s %d\n", msg, expr_str, file, line);
		//for (;;);
		char buf[256];
		sprintf(buf, "Assert failed: %s Expected: %s %s %d\n", msg, expr_str, file, line);

		HaltSystem(buf);
	}
}

void __assert_failed(int line, const char *file, const char *expr)
{
	char buf[256];
	sprintf(buf, "Assert failed: Expected: %s %s %d\n", expr, file, line);

	HaltSystem(buf);

}

LONG InterlockedAdd(LONG volatile *Addend, LONG Value)
{
	int oldVal;
	int dummy;
	__asm
	{
		mov edi, Addend
		mov edx, Value
		loop_point :
		mov eax, [edi]
		mov ecx, eax
		add ecx, edx
		lock cmpxchg [edi], ecx
		jnz loop_point

		mov oldVal, eax
		mov dummy, ecx
	}
	return oldVal;
}

LONG InterlockedAnd(LONG volatile *Destination, LONG Value)
{
	int oldVal;
	int dummy;
	__asm
	{
		mov edi, Destination
		mov edx, Value
loop_point:
		mov eax, [edi]
		mov ecx, eax
		and ecx, edx
		lock cmpxchg [edi], ecx
		jnz loop_point

		mov oldVal, eax
		mov dummy, ecx
	}
	return oldVal;
}

LONG InterlockedOr(LONG volatile *Destination, LONG Value)
{
	int oldVal;
	int dummy;
	__asm
	{
		mov edi, Destination
		mov edx, Value
		loop_point :
		mov eax, [edi]
			mov ecx, eax
			or ecx, edx
			lock cmpxchg[edi], ecx
			jnz loop_point

			mov oldVal, eax
			mov dummy, ecx
	}
	return oldVal;
}

LONG InterlockedCompareExchange(LONG volatile *Destination, LONG ExChange, LONG Comperand)
{
	int success;
	__asm
	{
		mov eax, ExChange
		mov ecx, Comperand
		mov edi, Destination
		lock cmpxchg[edi], ecx
		sete al
		and eax, ExChange
		mov success, eax

	}
	return success;	
}

LONG InterlockedDecrement(LONG volatile *Addend)
{
	int one = -1;
	__asm
	{
		mov eax, Addend
		mov ebx, one
		lock xadd[eax], ebx
	}

	return *Addend;
}

LONG InterlockedIncrement(LONG volatile *Addend)
{
	int one = 1;	
	__asm
	{		
		mov eax, Addend
		mov ebx, one
		lock xadd [eax], ebx
	}
	
	return *Addend;
}

LONG InterlockedXor(LONG volatile *Destination, LONG Value)
{
	int oldVal;
	int dummy;
	__asm
	{
		mov edi, Destination
		mov edx, Value
		loop_point :
		mov eax, [edi]
		mov ecx, eax
		xor ecx, edx
		lock cmpxchg[edi], ecx
		jnz loop_point

		mov oldVal, eax
		mov dummy, ecx
	}
	return oldVal;
}

DWORD GetLastError()
{
	return g_dwSysLastError;
}
DWORD SetLastError(DWORD dwErrorCode)
{
	return g_dwSysLastError = dwErrorCode;
}


bool GetLocalTime(LPSYSTEMTIME lpSystemTime)
{
	/* Checking whether we can read the time now or not according to some documentation the MSB in Status A will remain 1 (invalid time) only a millisecond*/
	int TimeOut;

	OutPortByte(RTC_INDEX_REG, RTC_STATUS_A);    //check status - read access
	TimeOut = 1000;
	while (InPortByte(RTC_VALUE_REG) & 0x80)
		if (TimeOut < 0)
			return false;
		else
			TimeOut--;

	OutPortByte(RTC_INDEX_REG, RTC_DAY);         //get day value
	lpSystemTime->wDay = InPortByte(RTC_VALUE_REG);
	OutPortByte(RTC_INDEX_REG, RTC_MONTH);       //get month value
	lpSystemTime->wMonth = InPortByte(RTC_VALUE_REG);
	OutPortByte(RTC_INDEX_REG, RTC_YEAR);        //get year
	lpSystemTime->wYear = InPortByte(RTC_VALUE_REG);

	OutPortByte(RTC_INDEX_REG, RTC_DAY_OF_WEEK); //get day of week - **** problem
	lpSystemTime->wDayOfWeek = InPortByte(RTC_VALUE_REG);

	OutPortByte(RTC_INDEX_REG, RTC_SECOND);
	lpSystemTime->wSecond = InPortByte(RTC_VALUE_REG);
	OutPortByte(RTC_INDEX_REG, RTC_MINUTE);
	lpSystemTime->wMinute = InPortByte(RTC_VALUE_REG);
	OutPortByte(RTC_INDEX_REG, RTC_HOUR);
	lpSystemTime->wHour = InPortByte(RTC_VALUE_REG);

	OutPortByte(RTC_INDEX_REG, RTC_STATUS_B);
	OutPortByte(RTC_VALUE_REG, 2);


	lpSystemTime->wYear = (lpSystemTime->wYear / 16) * 10 + (lpSystemTime->wYear % 16);
	lpSystemTime->wMonth = (lpSystemTime->wMonth / 16) * 10 + (lpSystemTime->wMonth % 16);
	lpSystemTime->wDay = (lpSystemTime->wDay / 16) * 10 + (lpSystemTime->wDay % 16);
	lpSystemTime->wHour = (lpSystemTime->wHour / 16) * 10 + (lpSystemTime->wHour % 16);
	lpSystemTime->wMinute = (lpSystemTime->wMinute / 16) * 10 + (lpSystemTime->wMinute % 16);
	lpSystemTime->wSecond = (lpSystemTime->wSecond / 16) * 10 + (lpSystemTime->wSecond % 16);

	return true;
}
/* Returns 1 on success and 0 on failue */
BYTE SetLocalTime(LPSYSTEMTIME lpSystemTime)
{
	/* Checking whether we can read the time now or not according to some documentation the MSB in Status A will   remain 1 (invalid time) only a millisecond*/
	int TimeOut;

	OutPortByte(RTC_INDEX_REG, RTC_STATUS_A);    //checking status -read access
	TimeOut = 1000;
	while (InPortByte(RTC_VALUE_REG) & 0x80)
		if (TimeOut < 0)
			return 0;
		else
			TimeOut--;

	OutPortByte(RTC_INDEX_REG, RTC_DAY);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wDay);
	OutPortByte(RTC_INDEX_REG, RTC_MONTH);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wMonth);
	OutPortByte(RTC_INDEX_REG, RTC_YEAR);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wYear);

	OutPortByte(RTC_INDEX_REG, RTC_SECOND);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wSecond);
	OutPortByte(RTC_INDEX_REG, RTC_MINUTE);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wMinute);
	OutPortByte(RTC_INDEX_REG, RTC_HOUR);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wHour);

	return 1;
}

//32비트 PE파일 이미지 유효성 검사
bool ValidatePEImage(void* image)
{
	IMAGE_DOS_HEADER* dosHeader = 0;
	IMAGE_NT_HEADERS* ntHeaders = 0;

	dosHeader = (IMAGE_DOS_HEADER*)image;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	if (dosHeader->e_lfanew == 0)
		return false;

	//NT Header 체크
	ntHeaders = (IMAGE_NT_HEADERS*)(dosHeader->e_lfanew + (uint32_t)image);
	if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
		return false;

	/* only supporting for i386 archs */
	if (ntHeaders->FileHeader.Machine != IMAGE_FILE_MACHINE_I386)
		return false;

	/* only support 32 bit executable images */
	if (!(ntHeaders->FileHeader.Characteristics & (IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_32BIT_MACHINE)))
		return false;

	/*
	Note: 1st 4 MB remains idenitity mapped as kernel pages as it contains
	kernel stack and page directory. If you want to support loading below 1MB,
	make sure to move these into kernel land
	*/

	//로드되는 프로세스의 베이스 주소는 0x00400000다. 
	//비쥬얼 스튜디오에서 속성=> 링커 => 고급의 기준주소 항목에서 확인 가능하다
	if ((ntHeaders->OptionalHeader.ImageBase < 0x400000) || (ntHeaders->OptionalHeader.ImageBase > 0x80000000))
		return false;

	/* only support 32 bit optional header format */
	if (ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
		return false;

	//유효한 32비트 PE 파일이다.
	return true;
}

void PrintCurrentTime()
{
	SYSTEMTIME time;
#ifdef SKY_EMULATOR
	return;
#else
	GetLocalTime(&time);
#endif
	char buffer[256];
	sprintf(buffer, "Current Time : %d/%d/%d %d:%d:%d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	printf("%s", buffer);
}

static char* sickpc = " (>_<) SkyOS Error!!\n\n";

char* szDisclame = "We apologize, SkyOS has encountered a problem and has been shut down\n\
to prevent damage to your computer. Any unsaved work might be lost.\n\
We are sorry for the inconvenience this might have caused.\n\n\
Please report the following information and restart your computer.\n\
The system has been halted.\n\n";

void HaltSystem(const char* errMsg)
{
	SkyConsole::MoveCursor(0, 0);
	SkyConsole::SetColor(ConsoleColor::White, ConsoleColor::Blue, false);
	SkyConsole::Clear();
	SkyConsole::Print(sickpc);

	SkyConsole::Print("*** STOP: %s", errMsg);
	//SkyDebugger::GetInstance()->TraceStackWithSymbol();
	for (;;);
}

extern "C" void printf(const char *fmt, ...)
{

	char buf[1024];

	va_list arglist;
	va_start(arglist, fmt);
	vsnprintf(buf, 1024, fmt, arglist);

#ifdef SKY_EMULATOR
	platformAPI._printInterface.sky_printf(buf);
#else
	SkyConsole::Print(buf);
#endif
}

extern unsigned int g_tickCount;
unsigned int GetTickCount()
{
	return g_tickCount;
}

void msleep(int ms)
{
	unsigned int ticks = ms + GetTickCount();
	while (ticks >= GetTickCount())
	{
		//Scheduler::GetInstance()->Yield();
	}
}