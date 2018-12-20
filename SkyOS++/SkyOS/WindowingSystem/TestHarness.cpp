//
// Copyright 1999-2002 Jeff Bush
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

//
// Simulate a mouse driver, keyboard driver, and raw frame buffer.
//

//#include <winsock2.h>
#include <windef.h>
#include "Rasterizer24bpp.h"
#include "MemorySurface.h"
#include "Surface.h"
#include "Screen.h"
#include "debug.h"
//#include "stdafx.h"
#include "resource.h"
#include "input.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
//#include <windows.h>

#define SHOW_DEBUG_WINDOW 0
#define CLIENT_TEST 1

#define MAX_CURSOR_WIDTH 32
#define MAX_CURSOR_HEIGHT 32
#define VIRTUAL_SCREEN_WIDTH 640
#define VIRTUAL_SCREEN_HEIGHT 480
#define INPUT_WINDOW_HEIGHT 20
#define FRAME_BUFFER_WINDOW_CLASS "FrameBufferWindow"
#define DEBUG_WINDOW_CLASS "DebugWindow"
#define MAX_DEBUG_ARGS 15
#define MAX_DEBUG_COMMANDS 32
#define DO_COMMAND (WM_USER + 2)

class VirtualFrameBuffer : public Screen {
public:
	VirtualFrameBuffer(HDC dc);
	void buildCursorData(HINSTANCE instance, HDC dc);
	void repaint(HWND hWnd);

	virtual void endTransaction();

	virtual void* lockBits();
	virtual void unlockBits();
	virtual int getStride() const;
	virtual Rasterizer *createRasterizer();
	virtual void setCursorPosition(int x, int y);
	virtual void setCursorShape(int width, int height, const char *colorBits, const char *maskBits);

private:
	void eraseCursor();
	void drawCursor();

	HBITMAP fScreenBitmap;

	int fLockCount;
	int fCursorX;
	int fCursorY;
	void *fSavedBackground;
	int fCursorWidth;
	int fCursorHeight;
	char *fCursorMask;
	char *fCursorColor;

	bool fDirty;
};

class ResourceBitmap : public MemorySurface {
public:
	ResourceBitmap(HINSTANCE instance, HDC dc, LPCTSTR bitmap, int width, int height);
};

struct InputEvent {
	InputEvent *next;
	enum NativeEventType type;
	int x;
	int y;
	bool lButtonDown;
	bool rButtonDown;
	int scanCode;
};

struct DebugCommand {
	const char *name;
	void (*callback)(int argc, const char **argv);
};

extern void clientMain();
extern int wsmain();
extern DWORD WINAPI ProtocolWorker(LPVOID param);

static LRESULT CALLBACK FrameBufferWndProc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK DebugWindowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK CommandOverrideWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL RegisterWindowClass(const char *name, WNDPROC, HINSTANCE, bool background);
static void EnqueueInputEvent(NativeEventType type, int x, int y, int scancode);
static InputEvent *DequeueInputEvent();
static void ExecuteCommand(const char *command);
static void showCommands(int argc, const char **argv);
static DWORD WINAPI ServerStartFunc(LPVOID param);
static DWORD WINAPI ClientStartFunc(LPVOID param);
static void buildCursorData(HINSTANCE instance, HDC dc, Screen *screen);

static VirtualFrameBuffer *screen;
static char *virtualScreenBuffer;
static HWND hFrameWindow;
static HWND hLogWindow;
static HWND hCommandWindow;
static InputEvent *inputQueueHead = NULL;
static InputEvent *inputQueueTail = NULL;
static CRITICAL_SECTION inputQueueLock;
static CRITICAL_SECTION bigLock;
static HANDLE inputQueueWait;
static bool lButtonDown = false;
static bool rButtonDown = false;
static WNDPROC oldCommandWndProc; // original wndproc for the command line
static HWND debugWindow;
static DebugCommand debugCommands[MAX_DEBUG_COMMANDS];
static Surface *bitmap;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HDC dc;
	WSADATA wsaData;
	int ret;

	// Initialize socket layer
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0) {
		_asm { int 3 }
		return 0;
	}

	// Set up frame buffer
	if (!RegisterWindowClass(FRAME_BUFFER_WINDOW_CLASS, (WNDPROC) FrameBufferWndProc, hInstance,
		false))
		return FALSE;

#if SHOW_DEBUG_WINDOW
	if (!RegisterWindowClass(DEBUG_WINDOW_CLASS, (WNDPROC) DebugWindowWndProc, hInstance, true))
		return FALSE;

	// Create the debug window
	debugWindow = CreateWindow(DEBUG_WINDOW_CLASS, "Debug Console", WS_OVERLAPPEDWINDOW,
		280, 180, 650, 500, NULL, NULL, hInstance, NULL);
	if (!debugWindow)
		return FALSE;

	// Create the frame buffer window
	hFrameWindow = CreateWindow(FRAME_BUFFER_WINDOW_CLASS, "Frame Buffer", WS_OVERLAPPED,
		10, 10, VIRTUAL_SCREEN_WIDTH + 6, VIRTUAL_SCREEN_HEIGHT + MAX_CURSOR_HEIGHT, NULL, NULL,
	  hInstance, NULL);
	if (!hFrameWindow)
		return FALSE;
#else
	// Create the frame buffer window
	hFrameWindow = CreateWindow(FRAME_BUFFER_WINDOW_CLASS, "Frame Buffer", WS_OVERLAPPED
		| WS_SYSMENU,
		10, 10, VIRTUAL_SCREEN_WIDTH + 6, VIRTUAL_SCREEN_HEIGHT + MAX_CURSOR_HEIGHT, NULL, NULL,
	  hInstance, NULL);
	if (!hFrameWindow)
		return FALSE;
#endif

	// Create a backing store for the virtual screen
	dc = GetDC(hFrameWindow);
	if (dc == NULL)
		return FALSE;

	screen = new VirtualFrameBuffer(dc);
	screen->buildCursorData(hInstance, dc);

	ShowWindow(hFrameWindow, nCmdShow);
	UpdateWindow(hFrameWindow);
	ShowCursor(FALSE);

#if SHOW_DEBUG_WINDOW
	ShowWindow(debugWindow, nCmdShow);
	UpdateWindow(debugWindow);
#endif

	bitmap = new ResourceBitmap(hInstance, dc, MAKEINTRESOURCE(IDB_BITMAP1), 48, 48);

	addDebugCommand("help", showCommands);

	// Initialize virtual input system
	inputQueueWait = CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
	InitializeCriticalSection(&inputQueueLock);
	InitializeCriticalSection(&bigLock);

	// Start main thread
	CloseHandle(CreateThread(NULL, 8192, ServerStartFunc, NULL, 0, NULL));

#if CLIENT_TEST
	Sleep(1000);	// Wait for server to initialize
	CloseHandle(CreateThread(NULL, 8192, ClientStartFunc, NULL, 0, NULL));
#endif

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

Screen *getScreen()
{
	return screen;
}

Surface *getBitmap()
{
	return bitmap;
}

bool getNextInputEvent(NativeEventType *outType, int *outParam1, int *outParam2, int *outParam3, bool wait)
{
	InputEvent *event;

	if (wait)
		WaitForSingleObject(inputQueueWait, INFINITE);

	EnterCriticalSection(&inputQueueLock);
	event = inputQueueHead;
	if (event != NULL) {
		inputQueueHead = inputQueueHead->next;
		if (inputQueueHead == NULL)
			inputQueueTail = NULL;
	}

	LeaveCriticalSection(&inputQueueLock);

	if (event != NULL) {
		*outType = event->type;
		switch (event->type) {
			case MOUSE_MOVE:
				*outParam1 = event->x;
				*outParam2 = event->y;
				break;

			case MOUSE_BUTTON:
				*outParam1 = event->x;
				*outParam2 = event->y;
				*outParam3 = (event->lButtonDown ? 1 : 0) | (event->rButtonDown ? 2 : 0);
				break;

			case KEY_DOWN:
				*outParam1 = event->scanCode;
				break;

			case KEY_UP:
				*outParam1 = event->scanCode;
				break;
		}

		free(event);
		return true;
	}

	return false;
}

void addDebugCommand(const char *name, void (*callback)(int argc, const char **argv))
{
	for (int i = 0; i < MAX_DEBUG_COMMANDS; i++) {
		if (debugCommands[i].name == NULL) {
			debugCommands[i].name = name;
			debugCommands[i].callback = callback;
			break;
		}
	}
}

void lprintf(const char *message, ...)
{
#if SHOW_DEBUG_WINDOW
	char formatted[1024];
	va_list args;

	va_start(args, message);
	vsprintf(formatted, message, args);
	va_end(args);

	SendMessage(hLogWindow, EM_SETSEL, 0x7fffffff, 0x7fffffff);
	SendMessage(hLogWindow, EM_REPLACESEL, 0, (LPARAM) formatted);
	SendMessage(hLogWindow, EM_LINESCROLL, 0, 1);
#endif
}

static LRESULT CALLBACK FrameBufferWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_PAINT:
			screen->repaint(hWnd);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_MOUSEMOVE:
			EnqueueInputEvent(MOUSE_MOVE, lParam & 0xffff, lParam >> 16, -1);
			break;

		case WM_LBUTTONDOWN:
			lButtonDown = true;
			EnqueueInputEvent(MOUSE_BUTTON, lParam & 0xffff, lParam >> 16, -1);
			break;

		case WM_LBUTTONUP:
			lButtonDown = false;
			EnqueueInputEvent(MOUSE_BUTTON, lParam & 0xffff, lParam >> 16, -1);
			break;

		case WM_RBUTTONDOWN:
			rButtonDown = true;
			EnqueueInputEvent(MOUSE_BUTTON, lParam & 0xffff, lParam >> 16, -1);
			break;

		case WM_RBUTTONUP:
			rButtonDown = false;
			EnqueueInputEvent(MOUSE_BUTTON, lParam & 0xffff, lParam >> 16, -1);
			break;

		case WM_KEYUP:
			EnqueueInputEvent(KEY_UP, -1, -1, wParam);
			break;

		case WM_KEYDOWN:
			EnqueueInputEvent(KEY_DOWN, -1, -1, wParam);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

static LRESULT CALLBACK DebugWindowWndProc(HWND hwnd,	// window handle
						  UINT message, 				// type of message
						  WPARAM wParam,				// additional information
						  LPARAM lParam)				// additional information
{
	char commandBuf[128];

	switch (message) {
		case WM_CREATE:
			// The log window displays output
			hLogWindow = CreateWindow("EDIT",		// predefined class
									NULL,		 // no window title
									WS_CHILD | WS_VISIBLE | WS_VSCROLL |
									ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
									0, 0, 0, 0,  // set size in WM_SIZE message
									hwnd,		 // parent window
									(HMENU) NULL,	// edit control ID
									(HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
									NULL);		 // pointer not needed
			SendMessage(hLogWindow, EM_SETREADONLY, TRUE, 0);

			// A command line at the bottom accepts commands
			hCommandWindow = CreateWindow("EDIT",		// predefined class
						  NULL, 		// no window title
						  WS_CHILD | WS_VISIBLE | ES_LEFT,
						  0, 0, 0, 0,  // set size in WM_SIZE message
						  hwnd, 		// parent window
						  (HMENU) NULL,	// edit control ID
						  (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
						  NULL);		// pointer not needed


			// Need to override window proc to intercept return key
			oldCommandWndProc = (WNDPROC) SetWindowLong(hCommandWindow,
				GWL_WNDPROC, (DWORD) CommandOverrideWinProc);
			return 0;

		case WM_SETFOCUS:
			SetFocus(hCommandWindow);
			return 0;

		case WM_SIZE:
			MoveWindow(hLogWindow,
						0, 0,				  // starting x- and y-coordinates
						LOWORD(lParam),		  // width of client area
						HIWORD(lParam) - INPUT_WINDOW_HEIGHT,		// height of client area
						TRUE);				  // repaint window

			MoveWindow(hCommandWindow,
						0,
						HIWORD(lParam) - INPUT_WINDOW_HEIGHT, // starting x- and y-coordinates
						LOWORD(lParam),		  // width of client area
						INPUT_WINDOW_HEIGHT, 		// height of client area
						TRUE);				  // repaint window
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case DO_COMMAND:
			SendMessage(hCommandWindow, WM_GETTEXT, (WPARAM) sizeof(commandBuf), (LPARAM)commandBuf);
			SendMessage(hCommandWindow, WM_SETTEXT, 0, (LPARAM) "");
			ExecuteCommand(commandBuf);
			break;

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return NULL;
}

LRESULT CALLBACK CommandOverrideWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_KEYDOWN:
			switch (wParam) {
				case VK_RETURN:
					// Bounce this message to the parent window
					SendMessage(debugWindow, DO_COMMAND, 0, 0);
					return 0;
			}
			break;

		case WM_KEYUP:
		case WM_CHAR:
			switch (wParam) {
				case VK_RETURN:
					return 0;
			}
	}

	// Call the original window procedure for default processing.
	return CallWindowProc(oldCommandWndProc, hwnd,
		msg, wParam, lParam);
}

static BOOL RegisterWindowClass(const char *name, WNDPROC wndProc, HINSTANCE instance,
	bool background)
{
	WNDCLASSEX wcex;

	// Create a class for the frame buffer window
	wcex.cbSize 	= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= wndProc;
	wcex.cbClsExtra 	= 0;
	wcex.cbWndExtra 	= 0;
	wcex.hInstance		= instance;
	wcex.hIcon			= LoadIcon(instance, (LPCTSTR)IDI_WINDOWSERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	if (background)
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	else
		wcex.hbrBackground	= NULL;

	wcex.lpszMenuName	= (LPCSTR)NULL;
	wcex.lpszClassName	= name;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

void acquireBigLock()
{
	EnterCriticalSection(&bigLock);
}

void releaseBigLock()
{
	screen->endTransaction();	// avoid a bunch of spurious updates

	LeaveCriticalSection(&bigLock);
}

static void EnqueueInputEvent(NativeEventType type, int x, int y, int scanCode)
{
	InputEvent *event = NULL;
	int length;

	EnterCriticalSection(&inputQueueLock);
	if (type == MOUSE_MOVE) {
		length = 0;
		for (event = inputQueueHead; event != NULL; event = event->next) {
			length++;
			if (event->type == MOUSE_MOVED) {
				// Already a mouse moved event queued, just update it
				event->x = x;
				event->y = y;
				break;
			}
		}
	}

	if (event == NULL) {
		// Couldn't find an existing event to merge this
		// with, create a new one
		event = (InputEvent*) malloc(sizeof(InputEvent));
		event->type = type;
		event->x = x;
		event->y = y;
		event->lButtonDown = lButtonDown;
		event->rButtonDown = rButtonDown;
		event->scanCode = scanCode;
		event->next = NULL;

		if (inputQueueHead == NULL)
			inputQueueHead = inputQueueTail = event;
		else {
			inputQueueTail->next = event;
			inputQueueTail = event;
		}
	}

	LeaveCriticalSection(&inputQueueLock);

	ReleaseSemaphore(inputQueueWait, 1, NULL);
}

static void ExecuteCommand(const char *command)
{
	char tmp[1024];
	const char *argv[MAX_DEBUG_ARGS];
	int argCount = 0;
	char *c;
	int i;

	if (strlen(command) > sizeof(tmp)) {
		lprintf("command too long\n");
		return;
	}

	strcpy(tmp, command);
	c = tmp;
	for (;;) {
		/* Skip to non-space character */
		while (isspace(*c))
			c++;

		if (*c == '\0')
			break;

		argv[argCount++] = c;

		/* Skip to space character */
		while (!isspace(*c)) {
			if (*c == '\0')
				goto done;

			c++;
		}

		*c++ = '\0';	/* Terminate this entry, proceed to next */
	}
done:
	if (argCount == 0)
		return;

	for (i = 0; i < MAX_DEBUG_COMMANDS; i++) {
		if (debugCommands[i].name == NULL)
			break;

		if (strcmp(debugCommands[i].name, argv[0]) == 0) {
			debugCommands[i].callback(argCount, argv);
			break;
		}
	}

	if (i == MAX_DEBUG_COMMANDS)
		lprintf("Unknown command %s", argv[0]);
}

static void showCommands(int argc, const char **argv)
{
	lprintf("Available commands:\n");
	for (int i = 0; i < MAX_DEBUG_COMMANDS; i++) {
		if (debugCommands[i].name == NULL)
			break;

		lprintf("  %s\n", debugCommands[i].name);
	}
}

static DWORD WINAPI ServerStartFunc(LPVOID param)
{
	wsmain();
	lprintf("wsmain has exited\n");

	return 0;
}

static DWORD WINAPI ClientStartFunc(LPVOID param)
{
	clientMain();
	lprintf("client main has exited\n");

	return 0;
}

VirtualFrameBuffer::VirtualFrameBuffer(HDC dc)
	:	Screen(Surface::kRGB24, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT),
		fCursorX(VIRTUAL_SCREEN_WIDTH / 2 - 16),
		fCursorY(VIRTUAL_SCREEN_HEIGHT / 2 - 16),
		fLockCount(0),
		fSavedBackground(NULL),
		fCursorColor(NULL),
		fCursorMask(NULL),
		fDirty(false)
{
	BITMAPINFO info;

	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = VIRTUAL_SCREEN_WIDTH + MAX_CURSOR_WIDTH;	// Add 32 pixels for cursor
	info.bmiHeader.biHeight = -(VIRTUAL_SCREEN_HEIGHT + MAX_CURSOR_HEIGHT);	// negative to make top down
	info.bmiHeader.biPlanes = 1;	// Must be one
	info.bmiHeader.biBitCount = 24;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biSizeImage = 0; // Must be zero for BI_RGB bitmaps
	info.bmiHeader.biXPelsPerMeter = 1024 * 3;
	info.bmiHeader.biYPelsPerMeter = 768 * 4;
	info.bmiHeader.biClrUsed = 0;	// Use maximum number of colors
	info.bmiHeader.biClrImportant = 0;	// All colors are required
	fScreenBitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, (void**) &virtualScreenBuffer,
		NULL, 0);
}

void* VirtualFrameBuffer::lockBits()
{
	if (!fDirty) {
		eraseCursor();
		fDirty = true;
	}

	return virtualScreenBuffer;
}

void VirtualFrameBuffer::unlockBits()
{
}

void VirtualFrameBuffer::endTransaction()
{
	if (fDirty) {
		fDirty = false;
		drawCursor();
		RedrawWindow(hFrameWindow, NULL, NULL, RDW_INVALIDATE);
	}
}

int VirtualFrameBuffer::getStride() const
{
	return (VIRTUAL_SCREEN_WIDTH + MAX_CURSOR_WIDTH) * 3;
}

Rasterizer *VirtualFrameBuffer::createRasterizer()
{
	return new Rasterizer24bpp(this);
}

void VirtualFrameBuffer::setCursorPosition(int x, int y)
{
	RECT dirtyRect;

	eraseCursor();

	if (x < 0)
		x = 0;
	else if (x >= VIRTUAL_SCREEN_WIDTH - 1)
		x = VIRTUAL_SCREEN_WIDTH - 1;

	if (y < 0)
		y = 0;
	else if (y >= VIRTUAL_SCREEN_HEIGHT - 1)
		y = VIRTUAL_SCREEN_HEIGHT - 1;

	dirtyRect.left = min(fCursorX, x);
	dirtyRect.right = max(fCursorX + MAX_CURSOR_WIDTH, x + MAX_CURSOR_WIDTH);
	dirtyRect.top = min(fCursorY, y);
	dirtyRect.bottom = max(fCursorY + MAX_CURSOR_HEIGHT, y + MAX_CURSOR_HEIGHT);

	fCursorX = x;
	fCursorY = y;

	drawCursor();
	RedrawWindow(hFrameWindow, &dirtyRect, NULL, RDW_INVALIDATE);
}

void VirtualFrameBuffer::setCursorShape(int width, int height, const char *colorBits,
	const char *maskBits)
{
	if (fCursorColor != NULL)
		eraseCursor();

	fCursorWidth = width;
	fCursorHeight = height;

	free(fSavedBackground);
	free(fCursorColor);
	free(fCursorMask);
	fSavedBackground = malloc(width * 3 * height);
	fCursorColor = (char*) malloc(width / 8 * height);
	fCursorMask = (char*) malloc(width / 8 * height);

	memcpy(fCursorColor, colorBits, width / 8 * height);
	memcpy(fCursorMask, maskBits, width / 8 * height);

	drawCursor();
}

void VirtualFrameBuffer::eraseCursor()
{
	// Restore what was originally behind the cursor
	char *destPtr = virtualScreenBuffer + fCursorX * 3 + fCursorY * ((VIRTUAL_SCREEN_WIDTH
		+ MAX_CURSOR_WIDTH) * 3);
	char *srcPtr = (char*) fSavedBackground;
	int screenSkip = (VIRTUAL_SCREEN_WIDTH + MAX_CURSOR_WIDTH) * 3;
	int cursorSkip = fCursorWidth * 3;

	for (int y = 0; y < MAX_CURSOR_WIDTH; y++) {
		memcpy(destPtr, srcPtr, fCursorWidth * 3);
		destPtr += screenSkip;
		srcPtr += cursorSkip;
	}
}

void VirtualFrameBuffer::drawCursor()
{
	if (fCursorColor == NULL)
		return;

	char *screenPtr = virtualScreenBuffer + fCursorX * 3 + fCursorY
		* ((VIRTUAL_SCREEN_WIDTH + MAX_CURSOR_WIDTH) * 3);
	char *backgroundPtr = (char*) fSavedBackground;
	int cursorByteWidth = fCursorWidth * 3;
	int skip = (VIRTUAL_SCREEN_WIDTH + MAX_CURSOR_WIDTH - fCursorWidth) * 3;
	int cursorOffset = 0;
	int cursorMask = 0x80;
	for (int y = 0; y < fCursorWidth; y++) {
		// Save what is behind the cursor
		memcpy(backgroundPtr, screenPtr, cursorByteWidth);
		backgroundPtr += cursorByteWidth;

		// Draw the cursor
		for (int x = 0; x < fCursorWidth; x++) {
			if (fCursorMask[cursorOffset] & cursorMask) {
				if (fCursorColor[cursorOffset] & cursorMask) {
					*screenPtr++ = 0xff;
					*screenPtr++ = 0xff;
					*screenPtr++ = 0xff;
				} else {
					*screenPtr++ = 0;
					*screenPtr++ = 0;
					*screenPtr++ = 0;
				}
			} else
				screenPtr += 3;

			cursorMask >>= 1;
			if (cursorMask == 0) {
				cursorOffset++;
				cursorMask = 0x80;
			}
		}

		screenPtr += skip;
	}
}

void VirtualFrameBuffer::buildCursorData(HINSTANCE instance, HDC dc)
{
	HBITMAP bm = LoadBitmap(instance, MAKEINTRESOURCE(IDB_BITMAP2));
	BITMAPINFO info;
	const unsigned char *cursorPtr;

	if (bm == NULL) {
		printf("error loading bitmap\n");
		return;
	}

	int width = 32;
	int height = 32;

	void *bits = malloc(width * 3 * height);
	memset(bits, 0, width * 3 * height);

	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = width;
	info.bmiHeader.biHeight = -height;	// negative to make top down
	info.bmiHeader.biPlanes = 1;	// Must be one
	info.bmiHeader.biBitCount = 24;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biSizeImage = 0; // Must be zero for BI_RGB bitmaps
	info.bmiHeader.biXPelsPerMeter = 1024 * 3;
	info.bmiHeader.biYPelsPerMeter = 768 * 4;
	info.bmiHeader.biClrUsed = 0;	// Use maximum number of colors
	info.bmiHeader.biClrImportant = 0;	// All colors are required

	if (GetDIBits(dc, bm, 0, height, bits, &info, DIB_RGB_COLORS) != height) {
		printf("Error getting DI bits\n");
		return;
	}

	// Convert bitmap data to cursor data
	char *maskData = (char*) malloc(width / 8 * height);
	char *colorData = (char*) malloc(width / 8 * height);

	memset(maskData, 0, width / 8 * height);
	memset(colorData, 0, width / 8 * height);

	cursorPtr = (const unsigned char*) bits;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// If this pixel is black, mark it transparent
			if (cursorPtr[0] != 0 || cursorPtr[1] != 0 || cursorPtr[2] != 0)
				maskData[x / 8 + y * width / 8] |= (0x80 >> x % 8);

			// if this pixel is white, mark it so, otherwise make it black
			// (black pixels in the final cursor are represented in this image
			// as dark gray).
			if (cursorPtr[0] == 0xff && cursorPtr[1] == 0xff && cursorPtr[2] == 0xff)
				colorData[x / 8 + y * width / 8] |= (0x80 >> x % 8);

			cursorPtr += 3;
		}
	}

	free(bits);

	setCursorShape(width, height, colorData, maskData);
}

void VirtualFrameBuffer::repaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HDC memDC;

	hdc = BeginPaint(hWnd, &ps);
	memDC = CreateCompatibleDC(hdc);
	SelectObject(memDC, fScreenBitmap);
	BitBlt(hdc, 0, 0, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT,
		memDC, 0, 0, SRCCOPY);
	DeleteObject(memDC);
	EndPaint(hWnd, &ps);
}

void __assert_failed(int line, const char *file, const char *expr)
{
	_asm { int 3 }
}

ResourceBitmap::ResourceBitmap(HINSTANCE instance, HDC dc, LPCTSTR bitmap, int width, int height)
	:	MemorySurface(Surface::kRGB24, width, height)
{
	HBITMAP bm = LoadBitmap(instance, bitmap);
	BITMAPINFO info;

	if (bm == NULL) {
		printf("error loading bitmap\n");
		return;
	}

	char *bits = (char*) lockBits();
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = width;
	info.bmiHeader.biHeight = -height;	// negative to make top down
	info.bmiHeader.biPlanes = 1;	// Must be one
	info.bmiHeader.biBitCount = 24;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biSizeImage = 0; // Must be zero for BI_RGB bitmaps
	info.bmiHeader.biXPelsPerMeter = 1024 * 3;
	info.bmiHeader.biYPelsPerMeter = 768 * 4;
	info.bmiHeader.biClrUsed = 0;	// Use maximum number of colors
	info.bmiHeader.biClrImportant = 0;	// All colors are required

	if (GetDIBits(dc, bm, 0, height, bits, &info, DIB_RGB_COLORS) != height)
		printf("Error getting DI bits\n");

	unlockBits();
}

