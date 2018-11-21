#pragma once
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#ifdef SKY_LUA_DLL_EXPORT
#define LUA_KERNEL_API   __declspec( dllexport ) 
#else
#define LUA_KERNEL_API   __declspec( dllimport ) 
#endif

class LUA_KERNEL_API LuaKernel
{
public:
	LuaKernel();
	~LuaKernel();

	bool Initialize(uint8* pFrameBuffer);
	bool HandleInterrupt(unsigned char scanCode);
	bool ExecuteCommand(char* command);
	lua_State *L;	
};

extern LUA_KERNEL_API LuaKernel*  testFunc11();