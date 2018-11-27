#include <stdio.h>
//#include "stdlib.h"
#include <string.h>
#include "local.h"
#include "windef.h"

GuiObject *add_image(GuiWindow * win, int x, int y, char **data)
{
	GuiObject *obj;
	int width, height, num_colors;

	check_window(win, "add_image");

	obj = (GuiObject *) malloc(sizeof(GuiObject));
	if (obj == NULL)
		error("Cannot allocate memory for image.");

	sscanf(data[0], "%d %d %d %*s\n", &width, &height, &num_colors);

	obj->win = win;
	obj->x = obj->x_min = x;
	obj->y = obj->y_min = y;
	obj->width = width;
	obj->height = height;
	obj->pressed = FALSE;
	obj->active = TRUE;
	obj->hide = FALSE;
	obj->wait_for_mouse = FALSE;
	obj->objclass = IMAGE;
	obj->u_data = 0;
	obj->align = ALIGN_CENTER;
	obj->x_max = x + width - 1;
	obj->y_max = y + height - 1;
	obj->bg_col1 = WIN_BACK;
	obj->info[0] = '\0';	/* make string length zero */
	set_object_callback(obj, object_cb);

	obj->data[0] = (char *) malloc(obj->width * obj->height);
	if (obj->data[0] == NULL)
		error("Unable to allocate data array in add_image().");
	obj->data[1] = NULL;

	add_object(obj);
	set_object_image_data(obj, data, 0, 0, FALSE, obj->bg_col1);

	return obj;
}


void update_image(GuiObject * obj)
{
	check_object(obj, "obj", "update_image");

	win_object(obj);
}


void show_image(GuiObject * obj)
{
	check_object(obj, "obj", "show_image");
	check_window(obj->win, "show_image");

	update_image(obj);
	show_window(obj->win);
}
