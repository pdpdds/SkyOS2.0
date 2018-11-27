#include <windef.h>
#include <stdio.h>
#include "local.h"

int check_window(GuiWindow *win, char *function)
{
	if (win == NULL) {
		//fprintf(stderr, "---> Warning: win = NULL in %s().\n", function);
		return TRUE;
	}
	
	return FALSE;
}


int check_object(GuiObject *obj, char *label, char *function)
{
	if (obj == NULL) {
		//fprintf(stderr, "---> Warning: %s = NULL in %s().\n", label, function);
		return TRUE;
	}
	
	return FALSE;
}
