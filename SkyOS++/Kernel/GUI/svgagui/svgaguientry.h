#include "svgagui.h"

extern GuiWindow *color_win;
extern GuiObject *color[3], *color_text;
extern int kleur;

void load_cb(GuiObject * obj, int data);
void save_cb(GuiObject * obj, int data);
void default_cb(GuiObject * obj, int data);
void create_color_win(GuiWinThread * win_thread);
int StartSampleGui();

