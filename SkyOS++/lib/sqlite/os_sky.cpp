#include "sqliteInt.h"
#if SQLITE_OS_OTHER
int sqlite3_os_init(void) 
{
	return SQLITE_OK;
}

int sqlite3_os_end(void) 
{

	return SQLITE_OK;
}

#endif