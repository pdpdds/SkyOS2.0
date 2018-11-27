#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "local.h"
#include "windef.h"
#include "PIT.h"

static GuiWindow *info_win;
static GuiObject *info = NULL;
static int showing_info = FALSE;


void show_info(GuiObject * obj)
{
	GuiWinThread *win_thread;

	if (check_object(obj, "obj", "show_info"))
		return;
	if (check_window(obj->win, "show_info"))
		return;

	win_thread = (obj->win)->win_thread;
	if (!win_thread->show_tips)
		return;
	if (showing_info)
		return;

	info_win = win_thread->tip_win;
	info_win->width = string_length(obj->info) + 6;
	info_win->x = mouse.x + cursor[mouse.cursor_id].cur_width;
	if (info_win->x + info_win->width > guiscreen.width - 1)
		info_win->x = guiscreen.width - 1 - info_win->width;
	info_win->y = mouse.y + cursor[mouse.cursor_id].cur_height;
	if (info_win->y + info_win->height > guiscreen.height - 1)
		info_win->y = guiscreen.height - 1 - info_win->height;

	kfree(info_win->data);
	info_win->data = (char *) malloc(info_win->width * info_win->height);
	if (info_win->data == NULL)
		error("Cannot allocate window buffers in show_info().");
	memset(info_win->data, BLACK, info_win->width * info_win->height);

	info = add_text(info_win, NORMAL_TEXT, 1, 1, obj->info);
	set_object_align(info, ALIGN_LEFT);
	set_object_color(info, INFO_BACK, BLACK, INFO_FORE);
	update_text(info);

	unhide_window(info_win, TRUE);
	bring_window_to_top(info_win);

	showing_info = TRUE;
}


void delete_info(void)
{	
	GuiWinThread *win_thread;

	if (check_object(info, "info", "delete_info"))
		return;
	if (check_window(info->win, "delete_info"))
		return;

	win_thread = (info->win)->win_thread;
	if (!win_thread->show_tips)
		return;
	if (!showing_info)
		return;

	hide_window(info_win, TRUE);

	delete_object(info);
	info = NULL;
	showing_info = FALSE;
}


void enable_info(GuiWinThread *win_thread, int enable)
{
	if (!enable && showing_info)
		delete_info();
	win_thread->show_tips = enable;
}
