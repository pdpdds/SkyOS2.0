#include "LuaKernel.h"
#include "lua.h"
#include "lua-bundle.h"
#include "SkyInterface.h"
#include "memory.h"
#include "string.h"
#include "luatinker.h"
#include "LuaStandardAlone.h"

uint8 *fbmem = nullptr;
uint8 *display_buffer = nullptr;
uint32 display_buffer_len = 0;
#define arraylen(array) (sizeof(array) / sizeof(array[0]))
char m_command[256];;
bool m_bCommandReady = false;

extern "C" void printf(const char* str, ...);

int clear_screen(lua_State *L)
{
	//SkyConsole::Clear();
	return 0;
}

static int putpixel(lua_State *l)
{
	uint32 x = lua_tonumber(l, 1);
	uint32 y = lua_tonumber(l, 2);
	uint32 r = lua_tonumber(l, 3);
	uint32 g = lua_tonumber(l, 4);
	uint32 b = lua_tonumber(l, 5);
	lua_pop(l, 5);
	const uint32 bytes_per_pixel = (32 / 8);
	// http://forum.osdev.org/viewtopic.php?p=77998&sid=d4699cf03655c572906144641a98e4aa#p77998
	uint8 *ptr =
		//&display_buffer[(y * modeinfo.BytesPerScanLine) + (x * bytes_per_pixel)];
		&display_buffer[(y * 1024 * 4) + (x * bytes_per_pixel)];

	const uint8 *display_buffer_end =
		//&display_buffer[(modeinfo.YResolution * modeinfo.BytesPerScanLine)];
		&display_buffer[768 * 1024*4];

	if (ptr < display_buffer_end)
	{
		ptr[0] = b;
		ptr[1] = g;
		ptr[2] = r;
		ptr[3] = 0;
	}
	else
	{
	//	HaltSystem("putpixel");
	}

	return 0;
}

static int swap_buffers(lua_State *l)
{
	//memcpy(fbmem, display_buffer, display_buffer_len);
	//clear_screen(l);
	return 0;
}

// http://lua-users.org/lists/lua-l/2003-12/msg00301.html
// http://lua-users.org/lists/lua-l/2002-12/msg00171.html
// http://lua-users.org/lists/lua-l/2011-06/msg00426.html
// http://lua-users.org/lists/lua-l/2010-03/msg00679.html

static void lua_hook(lua_State *l, lua_Debug *ar)
{
	lua_yield(l, 0);
}

static int lua_setmaskhook(lua_State *l)
{
	lua_State *t = lua_tothread(l, 1);
	int maskcount = lua_tointeger(l, 2);
	lua_pop(l, 2);
	if (t)
	{
		lua_sethook(t, lua_hook, LUA_MASKCOUNT, maskcount);
	}
	return 0;
}

static int lua_get_timer_ticks(lua_State *l)
{
	lua_pushinteger(l, platformAPI._processInterface.sky_get_tick_count());
	return 1;
}

static int lua_sleep(lua_State *l)
{
	platformAPI._processInterface.sky_ksleep(0);
	return 0;
}

static uint32 keyboard_scancode_queue[8] = { 0 };
static uint32 keyboard_scancode_queue_len = 0;
lua_State *g_AL;

int lua_get_keyboard_interrupt(lua_State *l)
{
	// disable interrupts
	platformAPI._processInterface.sky_enter_critical_section();


	if (m_bCommandReady == true)
	{			
		lua_pushstring(l, m_command);	
		m_bCommandReady = false;
	}	
	else
		lua_pushstring(l, nullptr);

	platformAPI._processInterface.sky_leave_critical_section();
	return 1;
}

static int lua_hlt(lua_State *l)
{
	//__asm hlt;
	return 0;
}

const char *errstr = NULL;


LuaKernel*  testFunc11()
{
	
	return new LuaKernel();
}

static int lua_loader(lua_State *l)
{
	size_t len;
	const char *modname = lua_tolstring(l, -1, &len);
	struct module *mod = NULL;
	for (int i = 0; i < arraylen(lua_bundle); ++i)
	{
		if (memcmp(modname, lua_bundle[i].name, len) == 0)
		{
			mod = &lua_bundle[i];
		}
	}
	if (!mod)
	{
		lua_pushnil(l);
		return 1;
	}
	if (luaL_loadbuffer(l, (const char*)mod->buf, mod->len, (const char*)mod->name) != LUA_OK)
	{
		errstr = lua_tostring(l, 1);
		//~ puts("luaL_loadstring: error");
		//trap();
	}
	int err = lua_pcall(l, 0, LUA_MULTRET, 0);
	if (err != LUA_OK)
	{
		errstr = lua_tostring(l, 1);
		//~ puts("lua_pcall: error");
		//trap();
	}
	if (!lua_istable(l, -1))
	{
		printf("not a table");
	}
	return 1;
}

LuaKernel::LuaKernel()
{
	
}


LuaKernel::~LuaKernel()
{
}

static void *l_alloc(void *ud, void *ptr, size_t osize, size_t nsize) {
	//(void)ud; (void)osize;  /* not used */

	if (nsize == 0) {

		if(ptr)
			kfree(ptr);
		return NULL;
	}
	else
		return krealloc(ptr, nsize);
}

extern int pmain(lua_State *L);

struct tag_argv
{
	char* name;
} argv[] =
{
	"LuaKernel",
};
int cpp_func(int arg1, int arg2)
{
	return arg1 + arg2;
}

static int cpp_int = 100;

bool LuaKernel::Initialize(uint8* pFrameBuffer)
{	

	/*char* args[]
	{
		"LuaKernel",
	};
	aamain(1, args);

	for (;;);*/
	L = lua_newstate(l_alloc, NULL);
	
	if (!L)
	{
		//puts("lua_newstate: error");
		return false;
	}

	luaopen_base(L);
	luaL_openlibs(L);

	display_buffer_len = (768 * 1024 * 4);
	display_buffer = pFrameBuffer;
	
	lua_pushnumber(L, 1024);
	lua_setglobal(L, "DISPLAY_WIDTH");
	lua_pushnumber(L, 768);
	lua_setglobal(L, "DISPLAY_HEIGHT");
	
	luatinker::def(L, "clear_screen", clear_screen);
	luatinker::def(L, "putpixel", putpixel);
	luatinker::def(L, "swap_buffers", swap_buffers);
	luatinker::def(L, "sleep", lua_sleep);

	luatinker::def(L, "setmaskhook", lua_setmaskhook);
	luatinker::def(L, "loader", lua_loader);
	luatinker::def(L, "get_timer_ticks", lua_get_timer_ticks);	
	luatinker::def(L, "get_keyboard_interrupt", lua_get_keyboard_interrupt);
	luatinker::def(L, "hlt", lua_hlt);
	
	if (luaL_loadfile(L, "luakernel.lua") != LUA_OK)
	{		
		errstr = lua_tostring(L, 1);	
		printf("error %str\n", errstr);
		for (;;);
		return false;
	}

	int err = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (err != LUA_OK)
	{
		printf("errorcode %d\n", err);
		for (;;);
		return false;
	}
	printf("success %d\n", err);
	for (;;);
	return true;
}

bool LuaKernel::HandleInterrupt(unsigned char scanCode)
{

	//u32 scancode = inb(0x60);
	if (keyboard_scancode_queue_len < arraylen(keyboard_scancode_queue))
	{
		keyboard_scancode_queue[keyboard_scancode_queue_len] = scanCode;
		keyboard_scancode_queue_len += 1;
	}

	return true;
}

bool LuaKernel::ExecuteCommand(char* command)
{
	if (strlen(command) == 0)
		return false;

	pushCommand(command);

	//kEnterCriticalSection();
	strcpy(m_command, command);
	m_bCommandReady = true;
	//kLeaveCriticalSection();

	return true;
}


