#include "KernelAPI.h"
#include "PlatformAPI.h"
#include "windef.h"
#include "stdarg.h"
#include "string.h"
#include "SkyConsole.h"

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