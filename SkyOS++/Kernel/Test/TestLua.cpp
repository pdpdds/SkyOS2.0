#include "TestLua.h"

#include "SkyMockInterface.h"
#include "I_LuaModule.h"
#include "SkyConsole.h"
#include "SkyOS.h"
#include "lua.h"
#include "lualib.h"
#include "luatinker.h"

I_LuaModule* pLuaModule = nullptr;

extern SKY_FILE_Interface g_FileInterface;
extern SKY_ALLOC_Interface g_allocInterface;
extern SKY_Print_Interface g_printInterface;

typedef I_LuaModule*(*PGetLuaModule)();

extern FILE* g_stdOut;
extern FILE* g_stdIn;
extern FILE* g_stdErr;

bool TestLua3(char* szCommand)
{
	g_printInterface.sky_stdin = g_stdIn;
	g_printInterface.sky_stdout = g_stdOut;
	g_printInterface.sky_stderr = g_stdErr;

	StorageManager::GetInstance()->SetCurrentFileSystemByID('L');

	if (pLuaModule != nullptr)
	{
		bool result = pLuaModule->DoFile(szCommand);

		if (result == false)
			SkyConsole::Print("Lua Exec Fail\n");

		return false;
	}

	MODULE_HANDLE hwnd = SkyModuleManager::GetInstance()->LoadModuleFromMemory("LUA3_DLL");

	if (hwnd == nullptr)
	{
		HaltSystem("LUA3_DLL Module Load Fail!!");
	}

	PSetSkyMockInterface SetSkyMockInterface = (PSetSkyMockInterface)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "SetSkyMockInterface");
	PGetLuaModule GetLuaModuleInterface = (PGetLuaModule)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "GetLuaModule");

	//����� ������ �÷��� �������� �������̽��� �ѱ��.
	SetSkyMockInterface(g_allocInterface, g_FileInterface, g_printInterface);

	if (!GetLuaModuleInterface)
	{
		HaltSystem("Memory Module Load Fail!!");
	}

	pLuaModule = GetLuaModuleInterface();

	if (pLuaModule == nullptr)
	{
		HaltSystem("Lua Module Creation Fail!!");
	}


	pLuaModule->InitLua();
	bool result = pLuaModule->DoFile(szCommand);
	//pLuaModule->CloseLua();

	if (result == false)
		SkyConsole::Print("Lua Exec Fail\n");

	return result;
}

int cpp_func(int arg1, int arg2)
{
	return arg1 + arg2;
}

static int cpp_int = 100;



void TestLua51(lua_State* L)
{

	luatinker::def(L, "cpp_func", cpp_func);
	luatinker::dofile(L, "sample1.lua");
	int result = luatinker::call<int>(L, "luafunc", 3, 4);
	printf("luafunc(3,4) = %d\n", result);
}

void TestLua52(lua_State* L)
{

	// LuaTinker �� �̿��ؼ� cpp_int �� Lua�� ����
	luatinker::set(L, "cpp_int", cpp_int);

	// sample1.lua ������ �ε�/�����Ѵ�.
	luatinker::dofile(L, "sample2.lua");

	// sample1.lua �� �Լ��� ȣ���Ѵ�.
	int lua_int = luatinker::get<int>(L, "lua_int");

	// lua_int �� ���
	printf("lua_int = %d\n", lua_int);

}

struct A
{
	A(int v) : value(v) {}
	int value;
};

struct base
{
	base() {}

	const char* is_base() { return "this is base"; }
};

class test : public base
{
public:
	test(int val) : _test(val) {}
	~test() {}

	const char* is_test() { return "this is test"; }

	void ret_void() {}
	int ret_int() { return _test; }
	int ret_mul(int m) const { return _test * m; }
	A get() { return A(_test); }
	void set(A a) { _test = a.value; }

	int _test;
};

//test g_test(11);

void TestLua53(lua_State* L)
{
	test g_test(11);

	// Lua ���ڿ� �Լ����� �ε��Ѵ�.- string ���
	luaopen_string(L);

	// base Ŭ������ Lua �� �߰��Ѵ�.
	luatinker::class_add<base>(L, "base");
	// base �� �Լ��� ����Ѵ�.
	luatinker::class_def<base>(L, "is_base", &base::is_base);

	// test Ŭ������ Lua �� �߰��Ѵ�.
	luatinker::class_add<test>(L, "test");
	// test �� base �� ��� �޾����� �˷��ش�.
	luatinker::class_inh<test, base>(L);

	// test Ŭ���� �����ڸ� ����Ѵ�.
	luatinker::class_con<test>(L, luatinker::constructor<test, int>);
	// test �� �Լ����� ����Ѵ�.
	luatinker::class_def<test>(L, "is_test", &test::is_test);
	luatinker::class_def<test>(L, "ret_void", &test::ret_void);
	luatinker::class_def<test>(L, "ret_int", &test::ret_int);
	luatinker::class_def<test>(L, "ret_mul", &test::ret_mul);
	luatinker::class_def<test>(L, "get", &test::get);
	luatinker::class_def<test>(L, "set", &test::set);
	luatinker::class_mem<test>(L, "_test", &test::_test);

	// Lua ���� ����ȣ g_test �� �����͸� ����Ѵ�.
	luatinker::set(L, "g_test", &g_test);

	// sample3.lua ������ �ε�/�����Ѵ�.
	luatinker::dofile(L, "sample3.lua");
}

void TestLua54(lua_State* L)
{

	// Lua ���̺��� �����ϰ� ���ÿ� Ǫ���Ѵ�.
	luatinker::table haha(L, "haha");

	// haha.value = 1 ���� �ִ´�.
	haha.set("value", 1);

	// table ���� table�� ����� �ִ´�.
	haha.set("inside", luatinker::table(L));

	// haha.inside �� �����͸� �������� �����Ѵ�.
	luatinker::table inside = haha.get<luatinker::table>("inside");

	// inside.value = 2 ���� �ִ´�.
	inside.set("value", 2);

	// sample4.lua ������ �ε�/�����Ѵ�.
	luatinker::dofile(L, "sample4.lua");

	// Lua ���� ������ haha.test ���� �д´�.
	const char* test = haha.get<const char*>("test");
	printf("haha.test = %s\n", test);

	// ������ ������� �ʰ� Lua ���ÿ� �� ���̺��� �����Ѵ�.(��������)
	luatinker::table temp(L);

	// �� ���̺�.name �� ���� �ִ´�.
	temp.set("name", "local table !!");

	// table�� �� ���ڷ� ����Ͽ� print_table �� ȣ���Ѵ�.
	luatinker::call<void>(L, "print_table", temp);

	// �Լ��� �����ϴ� table�� �޴´�.
	luatinker::table ret = luatinker::call<luatinker::table>(L, "return_table", "give me a table !!");
	printf("ret.name =\t%s\n", ret.get<const char*>("name"));

}

void show_error(const char* error)
{
	printf("_ALERT -> %s\n", error);
}

void TestLua55(lua_State* L)
{

	// lua_State* �� �����ִ� ������ ������ �����ش�.
	printf("%s\n", "-------------------------- current stack");
	luatinker::enum_stack(L);

	// ���ÿ� 1�� �о�ִ´�.
	lua_pushnumber(L, 1);

	// ���� ������ ������ �ٽ� ����Ѵ�.
	printf("%s\n", "-------------------------- stack after push '1'");
	luatinker::enum_stack(L);


	// sample5.lua ������ �ε�/�����Ѵ�.
	luatinker::dofile(L, "sample5.lua");

	// test_error() �Լ��� ȣ���Ѵ�.
	// test_error() �� ������ test_error_3() ȣ���� �õ��ϴ� ������ �߻���Ų��.
	// �Լ� ȣ���� �߻��� ������ printf()�� ���ؼ� ��µȴ�.
	printf("%s\n", "-------------------------- calling test_error()");
	luatinker::call<void>(L, "test_error");

	// test_error_3()�� �������� �ʴ� �Լ��̴�. ȣ�� ��ü�� �����Ѵ�.
	printf("%s\n", "-------------------------- calling test_error_3()");
	luatinker::call<void>(L, "test_error_3");

	// printf() ��� ������ �����ϴ� ���� ��� ��ƾ�� ����� �� �ִ�.
	// �� ����ó�� �Լ���1���� ��� ���ڿ��� �߻��� ������ �����ϰ� �ȴ�.
	// C++ ���� ����� ��� void function(const char*) ���°� �����ϴ�.
	luatinker::def(L, "_ALERT", show_error);

	luatinker::call<void>(L, "_ALERT", "test !!!");

	// test_error() �Լ��� ȣ���Ѵ�.
	// �Լ� ȣ���� �߻��� ������ Lua�� ��ϵ� _ALERT()�� ���ؼ� ��µȴ�.
	printf("%s\n", "-------------------------- calling test_error()");
	luatinker::call<void>(L, "test_error");
}

// �Լ� ���°� int(*)(lua_State*) �Ǵ� int(*)(lua_State*,T1) �� ��츸 lua_yield() �� �� �� �ִ�.
// �Լ� ���ڰ� �� �ʿ��� ��� lua_tinker.h �� "functor (non-managed)" �ڸ�Ʈ �κ��� �����ؼ� ���� �߰��� ��
int TestFunc(lua_State* L)
{
	printf("# TestFunc ������\n");
	return lua_yield(L, 0);
}

int TestFunc2(lua_State* L, float a)
{
	printf("# TestFunc2(L,%f) ������\n", a);
	return lua_yield(L, 0);
}

class TestClass
{
public:

	// �Լ� ���°� int(T::*)(lua_State*) �Ǵ� int(T::*)(lua_State*,T1) �� ��츸 lua_yield() �� �� �� �ִ�.
	// �Լ� ���ڰ� �� �ʿ��� ��� lua_tinker.h �� "class member functor (non-managed)" �ڸ�Ʈ �κ��� �����ؼ� ���� �߰��� ��
	int TestFunc(lua_State* L)
	{
		printf("# TestClass::TestFunc ������\n");
		return lua_yield(L, 0);
	}

	int TestFunc2(lua_State* L, float a)
	{
		printf("# TestClass::TestFunc2(L,%f) ������\n", a);
		return lua_yield(L, 0);
	}
};

void TestLua56(lua_State* L)
{
	// Lua ���ڿ� �Լ����� �ε��Ѵ�.- string ���
	luaopen_string(L);

	// TestFunc �Լ��� Lua �� ����Ѵ�.
	luatinker::def(L, "TestFunc", &TestFunc);
	luatinker::def(L, "TestFunc2", &TestFunc2);

	// TestClass Ŭ������ Lua �� �߰��Ѵ�.
	luatinker::class_add<TestClass>(L, "TestClass");
	// TestClass �� �Լ��� ����Ѵ�.
	luatinker::class_def<TestClass>(L, "TestFunc", &TestClass::TestFunc);
	luatinker::class_def<TestClass>(L, "TestFunc2", &TestClass::TestFunc2);

	// TestClass �� ���� ������ �����Ѵ�.
	TestClass g_test;
	luatinker::set(L, "g_test", &g_test);

	// sample3.lua ������ �ε��Ѵ�.
	luatinker::dofile(L, "sample6.lua");

	// Thread �� �����Ѵ�.
	lua_State *co = lua_newthread(L);
	lua_getglobal(co, "ThreadTest");
	// Thread �� �����Ѵ�.
	int result = 0;
	printf("* lua_resume() call\n");
	lua_resume(co, L, 0, &result);

	// Thread �� �ٽ� �����Ѵ�.
	printf("* lua_resume() call\n");
	lua_resume(co, L, 0, &result);

	// Thread �� �ٽ� �����Ѵ�.
	printf("* lua_resume() call\n");
	lua_resume(co, L, 0, &result);

	// Thread �� �ٽ� �����Ѵ�.
	printf("* lua_resume() call\n");
	lua_resume(co, L, 0, &result);


	// Thread �� �ٽ� �����Ѵ�.
	printf("* lua_resume() call\n");
	lua_resume(co, L, 0, &result);
}

bool TestLua5(char* szCommand)
{
	StorageManager::GetInstance()->SetCurrentFileSystemByID('L');

	if (szCommand == nullptr)
		return false;

	kEnterCriticalSection();
	lua_State* L;
	L = luaL_newstate();

	luaopen_base(L);

	if (strcmp(szCommand, "sample1.lua") == 0)
	{
		TestLua51(L);
	}

	if (strcmp(szCommand, "sample2.lua") == 0)
	{
		TestLua52(L);
	}

	if (strcmp(szCommand, "sample3.lua") == 0)
	{
		TestLua53(L);
	}

	if (strcmp(szCommand, "sample4.lua") == 0)
	{
		TestLua54(L);
	}

	if (strcmp(szCommand, "sample5.lua") == 0)
	{
		TestLua55(L);
	}
	if (strcmp(szCommand, "sample6.lua") == 0)
	{
		TestLua56(L);
	}

	lua_close(L);

	kLeaveCriticalSection();
	return false;
}

/*

kEnterCriticalSection();

SystemProfiler::GetInstance()->PrintMemoryState();
for (int i = 0; i < 100; i++)
{
lua_State* L;
L = luaL_newstate();
luaopen_base(L);

TestLua53(L);

lua_close(L);
}

SystemProfiler::GetInstance()->PrintMemoryState();

kLeaveCriticalSection();
*/