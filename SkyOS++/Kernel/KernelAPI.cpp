#include "KernelAPI.h"
#include "PlatformAPI.h"
#include "windef.h"
#include "stdarg.h"
#include "string.h"

extern "C" void printf(const char *fmt, ...)
{
	/*char buf[1024];

	va_list arglist;
	va_start(arglist, fmt);
	vsnprintf(buf, 1024, fmt, arglist);

	platformAPI._printInterface.sky_printf(buf);*/
}