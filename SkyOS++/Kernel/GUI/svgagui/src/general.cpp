#include "local.h"
#include <stdio.h>

int svgagui_initialized = FALSE, sleep_time = 1;


void set_sleep_time(int time)
{
	sleep_time = time;
}


void init_svgagui(void)
{
	guiscreen.data = NULL;
	#if 0
	guiscreen.image_data = NULL;
	#endif
	
	mouse.savescreen = NULL;
	mouse.visible = FALSE;
	mouse.num_cursors = 0;
	mouse.cursor_id = -1;	/* no mouse defined yet */
	
	set_sleep_time(0);
	svgagui_initialized = TRUE;
}


int GuiGetMessage(void)
{
	int message = FALSE;
	
	switch (guiscreen.type) {
		case SVGALIB : 
			message = get_svga_message();
			break;
#if 0
		case XLIB :
			message = get_X_message();
			break;
#endif
	}

	return message;
}


int GuiMouseGetButton(void)
{
	int button = FALSE;
	
	switch (guiscreen.type) {
		case SVGALIB : 
			button = get_svga_mouse_button();
			break;
#if 0			
		case XLIB :
			button = get_X_mouse_button();
			break;
#endif
	}

	return button;
}


char GuiKeyboardGetChar(void)
{
	char ch = '\0';
	
	switch (guiscreen.type) {
		case SVGALIB:
			ch = get_svga_keyboard_char();
			break;
#if 0
		case XLIB :
			ch = get_X_keyboard_char();
			break;
#endif
	}

	return ch;
}
