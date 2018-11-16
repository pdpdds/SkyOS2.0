#include "SkyOS.h"
#include "TestInteger.h"
#include "PureCallExtend.h"
#include "ZetPlane.h"
#include "jsmn.h"
#include "ctrycatch.h"
#include "I_Compress.h"
#include "wchar.h"
//#include "hash_map.h"

//���ͷ�Ʈ �ڵ鷯 �׽�Ʈ
void TestDivideByZero();
void TestBreakPoint();
void TestInvalidOpcode();

void TestInterrupt()
{
	//TestDivideByZero();
	//TestBreakPoint();
	TestInvalidOpcode();
}

int _divider = 0;
int _dividend = 100;
void TestDivideByZero()
{
	int result = _dividend / _divider;

	//����ó���� ���� �Ʒ� �ڵ尡 ����ȴٰ� �ص�
	//result ����� �������� ���� �ƴϴ�
	//���� ������ ����� ��ɾ�ܿ��� ���� ���� ���̸� 
	//�߰��������� ������ �κ��� �����ߴ� �ؼ� �������� ����� ����ϴ� ���� ������.

	if (_divider != 0)
		result = _dividend / _divider;

	SkyConsole::Print("Result is %d, divider : %d\n", result, _divider);
}

void funcBreakPoint(void)
{
	__asm {
		align 4
		__asm _emit 0xcc
		__asm _emit 0x00
		__asm _emit 0x00
		__asm _emit 0x00
	}
}

void TestBreakPoint()
{
	funcBreakPoint();
}

void TestInvalidOpcode()
{
	__asm {
		align 4
		__asm _emit 0xFF
		__asm _emit 0xFF
		__asm _emit 0xFF
		__asm _emit 0xFF
	}
}
//���ͷ�Ʈ �ڵ鷯 �׽�Ʈ ��


//C++ �׽�Ʈ 
void TestCPP14();
void TestPureFunctionCall();
void TestHeapCorruption();
void TestHeapCorruption2();
void TestInheritance();

void TestCPlusPlus()
{
	TestCPP14();
	//TestPureFunctionCall();
	//TestHeapCorruption();
	//TestHeapCorruption2();
	//TestInheritance();
}

void TestCPP14()
{
	SkyConsole::Print("\nCPP14 Test\n\n");

	auto func = [x = 5]() { return x; };
	auto n1 = func();
	SkyConsole::Print("Lamda n1:%d\n", n1);

	constexpr TestInteger size(10);
	int x[size];
	x[3] = 11;
	SkyConsole::Print("constexor x[3]:%d\n", x[3]);
}

void TestPureFunctionCall()
{
	SkyConsole::Print("\nPure Function Call Test\n\n");
	PureCallExtend pureCall;
}

void TestHeapCorruption()
{
	SkyConsole::Print("\nHeap Corruption Test\n\n");
	Plane* pPlane = new Plane();

	//����� Ǫ�� ����
	*((char*)pPlane + sizeof(Plane) + 1) = (char)0;

	delete pPlane;
}

void TestHeapCorruption2()
{
	SkyConsole::Print("\nHeap Corruption Test2\n\n");
	Plane* pPlane = new Plane();

	//�����Լ� ���̺� �����߸���
	memset(pPlane, 0, sizeof(Plane));

	delete pPlane;
}

void TestInheritance()
{
	SkyConsole::Print("\nClass Inheritance Test\n\n");

	int i = 100;
	Plane* pPlane = new ZetPlane();
	pPlane->SetX(i);
	pPlane->SetY(i + 5);

	pPlane->IsRotate();

	SkyConsole::Print("Plane X : %d, Plane Y : %d\n", pPlane->GetX(), pPlane->GetY());

	delete pPlane;
}
//C++ �׽�Ʈ ��

//���� ���̺귯�� �׽�Ʈ

void Testlist();
void TestJson();
void TestString();
void TestVector();
void TestStack();
void TestDeque();
void TestQueue();

using namespace std;

void TestCommonLibrary()
{

}

void TestQueue()
{
	queue queue;
	QueueNode* pNode = new QueueNode();
	pNode->_data = (void*)5;
	queue.Enqueue(pNode);

	QueueNode* pNode2 = new QueueNode();
	pNode2->_data = (void*)10;
	queue.Enqueue(pNode2);

	QueueNode* result = queue.Dequeue();
	delete result;
	result = queue.Dequeue();
	delete result;
}

void TestStack()
{
	stack<int> s;
	s.push(10);
	s.push(20);
	s.push(30);

	while (s.size() > 0)
	{
		int data = s.pop();
		SkyConsole::Print("%d\n", data);
	}
}

void TestVector()
{
	vector<int> vec;
	vec.push_back(5);
	vec.push_back(1);
	vec.push_back(3);
	vec.push_back(4);

	while (vec.size() > 0)
	{
		int data = vec.back();
		SkyConsole::Print("%d\n", data);
		vec.pop_back();
	}	
}

typedef struct tag_testStruct
{
	int j;
	int k;
}testStruct;

void Testlist()
{
	list<testStruct> fstlist;
	list<int> scndlist;
	int counter = 0;

	for (int i = 0; i <= 10; ++i) {
		testStruct a;
		a.j = i;
		a.k = i + 1;
		fstlist.push_back(a);
	}
	list<testStruct>::iterator iter = fstlist.begin();
	for (size_t i = 0; i < fstlist.size(); i++, iter++)
	{
		SkyConsole::Print("item 0x%d 0x%d done\n", ((testStruct)(*iter)).j, ((testStruct(*iter)).k));
	}

	SkyConsole::Print("done!!\n");
}

static const char *JSON_STRING =
"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
		strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

void TestJson()
{
	int i;
	int r;
	jsmn_parser p; //�ļ�
	jsmntok_t t[128]; //��ū ������ 128���� ����

	//�ļ��� �ʱ�ȭ�ϰ� ��Ʈ���� �Ľ��Ѵ�.
	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t) / sizeof(t[0]));
	if (r < 0) {
		SkyConsole::Print("Failed to parse JSON: %d\n", r);
		return;
	}

	// �ֻ��� ��Ҵ� ������Ʈ�� �����Ѵ�.
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		SkyConsole::Print("Object expected\n");
		return;
	}

	char buf[256];
	memset(buf, 0, 256);

	//������ ��Ʈ ������Ʈ�� ���� ������ ����.
	for (i = 1; i < r; i++) {
		if (jsoneq(JSON_STRING, &t[i], "user") == 0) 
		{
			//user�� value���� ��´�.
			memcpy(buf, JSON_STRING + t[i + 1].start, t[i + 1].end - t[i + 1].start);
			SkyConsole::Print("- User: %s\n", buf);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "admin") == 0)
		{
			//admin�� value���� ��´�.
			SkyConsole::Print("- Admin: %.*s\n", t[i + 1].end - t[i + 1].start,
				JSON_STRING + t[i + 1].start);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "uid") == 0)
		{
			//uid�� value���� ��´�.
			SkyConsole::Print("- UID: %.*s\n", t[i + 1].end - t[i + 1].start,
				JSON_STRING + t[i + 1].start);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "groups") == 0) 
		{
			int j;
			//groups Ű�� �迭���� ������.
			SkyConsole::Print("- Groups:\n");
			if (t[i + 1].type != JSMN_ARRAY) {
				continue;
			}
			//�迭�� �Ľ��ؼ� ��ū�� ����.
			for (j = 0; j < t[i + 1].size; j++) {
				jsmntok_t *g = &t[i + j + 2];
				SkyConsole::Print("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
			}
			i += t[i + 1].size + 1;
		}
		else {
			SkyConsole::Print("Unexpected key: %.*s\n", t[i].end - t[i].start,
				JSON_STRING + t[i].start);
		}
	}
}

void TestMap()
{
	map<int, int> hashmap;
	map<int, int> m10;
	map<int, int>::iterator it1;
	map<int, int>::iterator it2;
	
	m10[1] = 2;
	m10[2] = 4;
	m10[4] = 8;
	m10[3] = 6;
	m10[5] = 10;
	m10[6] = 12;
	m10[7] = 14;
	m10[8] = 16;
	
	int i = 0;
	for (it1 = m10.begin(); it1 != m10.end(); it1++) {
		//cout << (*it1).first << "   " << (*it1).second << endl;
		SkyConsole::Print("%d  %d\n", (*it1).first, (*it1).second);
		i++;
	}

	//cout << "ERASE BY KEY" << endl;
	m10.erase(3);
	i = 0;
	for (it1 = m10.begin(); it1 != m10.end(); it1++) {
		//cout << (*it1).first << "   " << (*it1).second << endl;
		SkyConsole::Print("%d  %d\n", (*it1).first, (*it1).second);
		i++;
	}
}

void TestString()
{
	std::string str("abc");
	SkyConsole::Print("%s\n", str.c_str());

	std::string str2 = str;
	str2 = "cdf";
	SkyConsole::Print("%s\n", str2.c_str());

	std::string a = "foo";
	std::string b = "bar";
	a = b;
	cout << a.c_str() << endl;	
}

void TestDeque()
{
	std::deque<int> a;

	for (int i = 1; i < 6; i++)
		a.push_front(i);

	for (int i = 0; i < 5; i++)
		SkyConsole::Print("%d\n", a[i]);
}

//���� ���̺귯�� �׽�Ʈ ��

#include "lua.h"
//#include "luadebug.h"
#include "lualib.h"

//������ġ �׽�Ʈ
void TestStorage(const char* filename, char driveLetter)
{
	StorageManager::GetInstance()->SetCurrentFileSystemByID(driveLetter);

	FILE* pFile = fopen(filename, "r");
	
	if (pFile != NULL)
	{
		SkyConsole::Print("Handle ID : %d\n", pFile->_id);

		BYTE* buffer = new BYTE[512];
		memset(buffer, 0, 512);
		int ret = fread(buffer, 511, 1, pFile);

		if (ret > 0)
			SkyConsole::Print("%s [%d]\n", buffer, ret);

		fclose(pFile);		

		delete buffer;
	}

	SkyConsole::Print("Test End\n");
	for (;;);
}

void TestFPU()
{
	float sampleFloat = 0.3f;

	sampleFloat *= 5.482f;

	SkyConsole::Print("sample Float Value %f\n", sampleFloat);
}

//Try Catch �׽�Ʈ

void throwArgumentException() {
	//puts("Function reached.");
	throw(ArgumentException, (char*)"Ooh! Some ArgumentException was thrown. ");
}

void TestTryCatch()
{
	try {
		throwArgumentException();
	}
	catch (ArgumentException) {
		//	puts("ArgumentException block reached");
		if (__ctrycatch_exception_message_exists)
			SkyConsole::Print("message: %s\n", __ctrycatch_exception_message);
	}
	finally {
		//puts("finally block reached");
		SkyConsole::Print("Finally!!\n");
	}
}

void TestNullPointer()
{
	ZetPlane* pPlane = 0;
	pPlane->IsRotate();
}

typedef I_Compress* (*PGetEasyCompress)();

char easyTestBuffer[] = "Sky OS Compression Test!!";
void TestEasyZLib()
{
	//Load Hangul Engine
	StorageManager::GetInstance()->SetCurrentFileSystemByID('L');
	void* hwnd = SkyModuleManager::GetInstance()->LoadModule("zlib.dll");	
	PGetEasyCompress GetEasyCompress = (PGetEasyCompress)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "GetEasyCompress");
	
	if (!GetEasyCompress)
	{
		HaltSystem("HanguleMint64Engine Module Load Fail!!");
	}

	I_Compress* pEasyCompress = GetEasyCompress();

	char* destBuffer = new char[256];
	long destBufferLen = 256;
	long nSrcLen = sizeof(easyTestBuffer);

	char* decompressedBuffer = new char[256];
	long decompressedLen = 256;

	memset(destBuffer, 0, 256);
	memset(decompressedBuffer, 0, 256);

	SkyConsole::Print("text : %s\n", easyTestBuffer);
	
	//�����Ѵ�.
	if (0 != pEasyCompress->Compress((unsigned char* )destBuffer, &destBufferLen, ( unsigned char* )easyTestBuffer, nSrcLen))
	{
		HaltSystem("easyzlib test fail!!");
	}
	SkyConsole::Print("Compressed : Src Size %d, Dest Size %d\n", nSrcLen, destBufferLen);

	//������ �����Ѵ�. 
	if (0 != pEasyCompress->Decompress((unsigned char*)decompressedBuffer, &decompressedLen, ( unsigned char*)destBuffer, destBufferLen))
	{
		HaltSystem("easyzlib test fail!!");
	}
	SkyConsole::Print("Decompressed : Src Size %d, Dest Size %d\n", destBufferLen, decompressedLen);
	SkyConsole::Print("result : %s\n", decompressedBuffer);

	delete destBuffer;
	delete decompressedBuffer;
}

bool TestMemoryModule(const char* moduleName)
{
	void* hwnd = SkyModuleManager::GetInstance()->LoadModule(moduleName);

	if (hwnd == nullptr)
	{
		HaltSystem("Memory Module Load Fail!!");
	}

	PGetDLLInterface GetDLLInterface = (PGetDLLInterface)SkyModuleManager::GetInstance()->GetModuleFunction((MODULE_HANDLE)(hwnd), "GetDLLInterface");

	if (!GetDLLInterface)
	{
		HaltSystem("Memory Module Load Fail!!");
	}

	const DLLInterface* dll_interface = GetDLLInterface();

	int sum = dll_interface->AddNumbers(5, 6);

	SkyConsole::Print("AddNumbers(5, 6): %d\n", sum);

	if (false == SkyModuleManager::GetInstance()->UnloadModule((MODULE_HANDLE)(hwnd)))
		HaltSystem("UnloadDLL() failed!\n");

	return true;
}

#include "SDL.H"
#include "I_ImageInterface.h"
//extern "C" __declspec(dllimport) I_ImageInterface* GetImageInterface();
void TestSkySDL(int width, int height, int bpp)
{
	int screen_w;
	int screen_h;
	SDL_Surface *screen;
	SDL_Window *pWindow;
	SDL_Renderer *pRenderer;
	SDL_Texture *pTexture;
	//GetImageInterface();
	//������� �������� ����
	
	if (SDL_CreateWindowAndRenderer(width, height, 0, &pWindow, &pRenderer) < 0)
	{
		//std::cout << "SDL_CreateWindowAndRenderer Error: " << SDL_GetError() << std::endl;
		return;
	}
	

	SDL_GetWindowSize(pWindow, &screen_w, &screen_h);

	
	screen = SDL_CreateRGBSurface(0, screen_w, screen_h, bpp,
		0,
		0,
		0,
		0);

	if (screen == 0)
	{
		std::cout << "SDL_CreateRGBSurface Error: " << SDL_GetError() << std::endl;
		return;
	}

	if (bpp == 32)
	{
		pTexture = SDL_CreateTexture(pRenderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,
			screen_w, screen_h);
	}
	else if (bpp == 24)
	{
		pTexture = SDL_CreateTexture(pRenderer,
			SDL_PIXELFORMAT_RGB888,
			SDL_TEXTUREACCESS_STREAMING,
			screen_w, screen_h);
	}
	else if (bpp == 16)
	{
		pTexture = SDL_CreateTexture(pRenderer,
			SDL_PIXELFORMAT_RGB565,
			SDL_TEXTUREACCESS_STREAMING,
			screen_w, screen_h);
	}
	else if (bpp == 8)
	{
		pTexture = SDL_CreateTexture(pRenderer,
			SDL_PIXELFORMAT_RGB332,
			SDL_TEXTUREACCESS_STREAMING,
			screen_w, screen_h);
	}

	if (pRenderer == 0)
	{
		std::cout << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
		return;
	}

	if (pTexture == 0)
	{
		SDL_DestroyRenderer(pRenderer);
		SDL_DestroyWindow(pWindow);
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return;
	}
}

#include <ft2build.h>
#include FT_FREETYPE_H


#define WIDTH   640
#define HEIGHT  480


/* origin is the upper left corner */
unsigned char image[HEIGHT][WIDTH];


/* Replace this function with something useful. */

void
draw_bitmap(FT_Bitmap*  bitmap,
	FT_Int      x,
	FT_Int      y)
{
	FT_Int  i, j, p, q;
	FT_Int  x_max = x + bitmap->width;
	FT_Int  y_max = y + bitmap->rows;


	/* for simplicity, we assume that `bitmap->pixel_mode' */
	/* is `FT_PIXEL_MODE_GRAY' (i.e., not a bitmap font)   */

	for (i = x, p = 0; i < x_max; i++, p++)
	{
		for (j = y, q = 0; j < y_max; j++, q++)
		{
			if (i < 0 || j < 0 ||
				i >= WIDTH || j >= HEIGHT)
				continue;

			image[j][i] |= bitmap->buffer[q * bitmap->width + p];
		}
	}
}


void PutPixel(ULONG x, ULONG y, ULONG col)
{
	ULONG* lfb = (ULONG*)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr;
	lfb[(y * 1024) + x] = col;
}



void
show_image(void)
{
	int  i, j;
	

	for (i = 0; i < HEIGHT; i++)
	{
		for (j = 0; j < WIDTH; j++)
		{
			
			if (image[i][j] != 0)
				PutPixel(j, i, 0xFFFFFFFF);
			else
				PutPixel(j, i, image[i][j]);
		}
	}
}

bool TestFreeType(char* szfilename, wchar_t* sztext)
{
	FT_Library    library;
	FT_Face       face;

	FT_GlyphSlot  slot;
	FT_Matrix     matrix;                 /* transformation matrix */
	FT_Vector     pen;                    /* untransformed origin  */
	FT_Error      error;

	char*         filename;
	wchar_t*         text;

	double        angle;
	int           target_height;
	int           n, num_chars;


	filename = szfilename;                           /* first argument     */
	text = sztext;                        /* second argument    */
	num_chars = wcslen(text);
	angle = (25.0 / 360) * 3.14159 * 2;      /* use 25 degrees     */
	target_height = HEIGHT;

	error = FT_Init_FreeType(&library);              /* initialize library */
													 /* error handling omitted */

	if (error)
		return false;

	error = FT_New_Face(library, filename, 0, &face);/* create face object */
													 /* error handling omitted */
	if (error)
		return false;
													 /* use 50pt at 100dpi */
	error = FT_Set_Char_Size(face, 50 * 64, 0,
		100, 0);                /* set character size */
								/* error handling omitted */

								/* cmap selection omitted;                                        */
								/* for simplicity we assume that the font contains a Unicode cmap */
	if (error)
		return false;

	slot = face->glyph;

	/* set up matrix */
	matrix.xx = (FT_Fixed)(cos(angle) * 0x10000L);
	matrix.xy = (FT_Fixed)(-sin(angle) * 0x10000L);
	matrix.yx = (FT_Fixed)(sin(angle) * 0x10000L);
	matrix.yy = (FT_Fixed)(cos(angle) * 0x10000L);

	/* the pen position in 26.6 cartesian space coordinates; */
	/* start at (300,200) relative to the upper left corner  */
	pen.x = 300 * 64;
	pen.y = (target_height - 200) * 64;

	for (n = 0; n < num_chars; n++)
	{
		/* set transformation */
		FT_Set_Transform(face, &matrix, &pen);

		FT_UInt glyph_index = FT_Get_Char_Index(face, text[n]);

		error = FT_Load_Glyph(
			face,          /* handle to face object */
			glyph_index,   /* glyph index           */
			FT_LOAD_RENDER);  /* load flags, see below */

		/* load glyph image into the slot (erase previous one) */
		//error = FT_Load_Char(face, text[n], FT_LOAD_RENDER);
		if (error)
			continue;                 /* ignore errors */

									  /* now, draw to our target surface (convert position) */
		draw_bitmap(&slot->bitmap,
			slot->bitmap_left,
			target_height - slot->bitmap_top);

		/* increment pen position */
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
	}

	//show_image();

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	return error;
}

