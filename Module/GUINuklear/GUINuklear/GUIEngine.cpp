#include "GUIEngine.h"
#include "windef.h"
#include "string.h"
#include "memory.h"
#include "SkyInterface.h"

#define NK_IMPLEMENTATION
#include "nuklear.h"



GUIEngine::GUIEngine()
{
	
}


GUIEngine::~GUIEngine()
{
}

struct nk_font *font;
struct nk_font_atlas atlas;

bool GUIEngine::Initialize() 
{
	nk_context* pCtx = new nk_context;
	nk_user_font* pFont = new nk_user_font;


	
	int MAX_MEMORY = 1024;
	nk_init_fixed(pCtx, (void*)calloc(1, MAX_MEMORY), MAX_MEMORY, pFont);

	enum { EASY, HARD };
	static int op = EASY;
	static float value = 0.6f;
	static int i = 20;

	if (nk_begin(pCtx, "Show", nk_rect(50, 50, 220, 220),
		NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE)) {
		/* fixed widget pixel width */
		nk_layout_row_static(pCtx, 30, 80, 1);
		if (nk_button_label(pCtx, "button")) {
			/* event handling */
		}

		/* fixed widget window ratio width */
		nk_layout_row_dynamic(pCtx, 30, 2);
		if (nk_option_label(pCtx, "easy", op == EASY)) op = EASY;
		if (nk_option_label(pCtx, "hard", op == HARD)) op = HARD;

		/* custom widget pixel width */
		nk_layout_row_begin(pCtx, NK_STATIC, 30, 2);
		{
			nk_layout_row_push(pCtx, 50);
			nk_label(pCtx, "Volume:", NK_TEXT_LEFT);
			nk_layout_row_push(pCtx, 110);
			nk_slider_float(pCtx, 0, &value, 1.0f, 0.1f);
		}
		nk_layout_row_end(pCtx);
	}
	nk_end(pCtx);

	return true;
}
void GUIEngine::Update(float deltaTime) 
{
	
}

void GUIEngine::SetLinearBuffer(LinearBufferInfo& linearBufferInfo)
{
	
}


bool GUIEngine::PutKeyboardQueue(KEYDATA* pData)
{
	return false;
}

bool  GUIEngine::PutMouseQueue(MOUSEDATA* pData)
{
	return false;
}