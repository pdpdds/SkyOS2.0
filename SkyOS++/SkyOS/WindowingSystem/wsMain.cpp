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

#include <windef.h>
#include "BlankWindow.h"
#include "debug.h"
#include "GraphicsContext.h"
#include "input.h"
#include "MemorySurface.h"
#include "NoisyWindow.h"
#include "Rasterizer.h"
#include "Rasterizer24bpp.h"
#include "RootWindow.h"
#include "RoundWindow.h"
#include "Screen.h"
#include "ScribbleWindow.h"
#include "ServerConnection.h"
#include "ServerWindow.h"
#include "TitledWindow.h"
#include "WindowManager.h"

Screen *getScreen();
Surface *getBitmap();

static Screen *frameBuffer = NULL;
WindowManager *windowManager;

static void listWindows(int argc, const char **argv)
{
	windowManager->listWindows();
}

class RegionDisplayer : public Clipper {
public:
	RegionDisplayer(Rasterizer *rasterizer, int color)
		:	fRasterizer(rasterizer),
			fColor(color)
	{
	}

	virtual void operator()(int left, int top, int right, int bottom);

private:
	Rasterizer *fRasterizer;
	int fColor;
};

void RegionDisplayer::operator()(int left, int top, int right, int bottom)
{
	fRasterizer->strokeLine(left, top, right, top, fColor);
	fRasterizer->strokeLine(left, top, left, bottom, fColor);
	fRasterizer->strokeLine(right, top, right, bottom, fColor);
	fRasterizer->strokeLine(left, bottom, right, bottom, fColor);
	fRasterizer->strokeLine(left, top, right, bottom, fColor);
	fRasterizer->strokeLine(left, bottom, right, top, fColor);
}

void displayRegion(const Region &region, int color)
{
	Rasterizer *rasterizer = frameBuffer->createRasterizer();
	RegionDisplayer disp(rasterizer, color);
	region.clip(0, 0, frameBuffer->getWidth() - 1, frameBuffer->getHeight() - 1,
		disp);

	delete rasterizer;
}

// Big lock is a hack, related to the way we handle input on windows.
extern void acquireBigLock();
extern void releaseBigLock();

static Rect nextWindowPosition(10, 10, 300, 300);

void spawnWindow(Surface *bitmap, int type)
{
	Rect windowRect(nextWindowPosition);
	nextWindowPosition.offsetBy(20, 20);

	TitledWindow *title = new TitledWindow(windowRect);

	windowRect.insetBy(BORDER_WIDTH, BORDER_WIDTH);
	windowRect.top += TITLE_HEIGHT - BORDER_WIDTH;
	ServerWindow *window;

	if (type == 0)
		window = new BlankWindow(windowRect, 0xff, bitmap);
	else if (type == 1)
		window = new NoisyWindow(windowRect, bitmap);
	else if (type == 3)
		window = new ScribbleWindow(windowRect);

	title->addChild(window);
	windowManager->newWindow(title);
}

extern Region* kInfinite;
int wsmain()
{
	kInfinite = new Region(-0x7fff, -0x7fff, 0x7fff, 0x7fff);
	frameBuffer = getScreen();

	windowManager = new WindowManager(frameBuffer);

	int width = frameBuffer->getWidth();
	int height = frameBuffer->getHeight();

	addDebugCommand("windows", listWindows);

	Rect screenBounds(0, 0, frameBuffer->getWidth(), frameBuffer->getHeight());

	RootWindow rootwin(screenBounds.right - 1, screenBounds.bottom - 1);
	windowManager->setRootWindow(&rootwin);


	ServerConnection::startServer();

	spawnWindow(0, 0);
	//spawnWindow(getBitmap(), 0);
	//spawnWindow(0, 3);

	acquireBigLock();
	while (true) {
		windowManager->doRepaint();
		releaseBigLock();

		NativeEventType type;
		int param1;
		int param2;
		int param3;
		if (getNextInputEvent(&type, &param1, &param2, &param3, true)) {
			acquireBigLock();
			if (type == MOUSE_MOVE) {
				frameBuffer->setCursorPosition(param1, param2);
				windowManager->handleMouseMove(param1, param2);
			} else if (type == MOUSE_BUTTON) {
				windowManager->handleMouseButton(param1, param2, param3);
				windowManager->doRepaint();
			} else if (type == KEY_DOWN)
				windowManager->handleKeyDown(param1);
			else if (type == KEY_UP)
				windowManager->handleKeyUp(param1);
		} else
			acquireBigLock();
	}

	return 0;
}
