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

#include "TitledWindow.h"
#include "Button.h"
#include "WindowManager.h"

static const Rect kCloseButtonRect(4, 4, 14, 14);
static const Rect kMaximizeButtonRect(18, 4, 28, 14);

static void closeWindow(void *win);
static void maximizeWindow(void *win);

extern WindowManager *windowManager;

TitledWindow::TitledWindow(const Rect &frame)
	:	ServerWindow(frame)
{
	setDraggable(true);

	Rect r(kCloseButtonRect);
	r.offsetBy(frame.left, frame.top);
	Button *button = new Button(r);
	button->setCallback(closeWindow, this);
	addChild(button);

	r = kMaximizeButtonRect;
	r.offsetBy(frame.left, frame.top);
	button = new Button(r);
	button->setCallback(maximizeWindow, this);
	addChild(button);
}

#define PLAIN_JANE 0

void TitledWindow::paint(GraphicsContext &gc)
{
    Rect fr = frame();
	fr.offsetTo(0, 0);

	// Frame
    gc.setColor(0x808080);

	#define HIGHLIGHT 0xa0a0a0
	#define NEUTRAL 0x808080
	#define SHADOW 0x606060

	// Left side
	gc.setColor(HIGHLIGHT);
	gc.strokeLine(0, 0, 0, fr.bottom);
	gc.setColor(NEUTRAL);
	gc.strokeLine(1, 1, 1, fr.bottom - 1);
	gc.setColor(SHADOW);
	gc.strokeLine(2, 2, 2, fr.bottom - 2);

	// Right side
	gc.setColor(SHADOW);
	gc.strokeLine(fr.right, 0, fr.right, fr.bottom);
	gc.setColor(NEUTRAL);
	gc.strokeLine(fr.right - 1, 1, fr.right - 1, fr.bottom - 1);
	gc.setColor(HIGHLIGHT);
	gc.strokeLine(fr.right - 2, 2, fr.right - 2, fr.bottom - 2);

	// Bottom
	gc.setColor(SHADOW);
	gc.strokeLine(0, fr.bottom, fr.right, fr.bottom);
	gc.setColor(NEUTRAL);
	gc.strokeLine(1, fr.bottom - 1, fr.right - 1, fr.bottom - 1);
	gc.setColor(HIGHLIGHT);
	gc.strokeLine(2, fr.bottom - 2, fr.right - 2, fr.bottom - 2);

	// Top
	gc.setColor(HIGHLIGHT);
	gc.strokeLine(0, 0, fr.right, 0);
	gc.setColor(NEUTRAL);
	gc.strokeLine(1, 1, fr.right - 1, 1);
	gc.setColor(SHADOW);
	gc.strokeLine(2, 2, fr.right - 2, 2);

	// Divider
	int dividerY = TITLE_HEIGHT - BORDER_WIDTH;
	gc.setColor(HIGHLIGHT);
	gc.strokeLine(2, dividerY, fr.right - 2, dividerY);
	gc.setColor(NEUTRAL);
	gc.strokeLine(1, dividerY + 1, fr.right - 1, dividerY + 1);
	gc.setColor(SHADOW);
	gc.strokeLine(2, dividerY + 2, fr.right - 2, dividerY + 2);

#if PLAIN_JANE
	if (isFocused())
		gc.setColor(0x8080ff);
	else
		gc.setColor(0xb0b0b0);

	gc.fillRect(fr.left + BORDER_WIDTH, BORDER_WIDTH, fr.right - BORDER_WIDTH,
		TITLE_HEIGHT - BORDER_WIDTH);
#else
	// Make a gradient
	bool focused = isFocused();
	for (int x = BORDER_WIDTH; x <= fr.right - BORDER_WIDTH; x++) {
		int color = (x - BORDER_WIDTH) * 255 / (fr.right - BORDER_WIDTH * 2);
		if (focused)
			gc.setColor(0xff + (color << 8) + (color << 16));
		else
			gc.setColor(color + (color << 8) + (color << 16));

		gc.strokeLine(x, BORDER_WIDTH, x, TITLE_HEIGHT - BORDER_WIDTH);
	}
#endif
}

static void closeWindow(void *win)
{
	windowManager->closeWindow((ServerWindow*) win);
}

static void maximizeWindow(void *win)
{
	windowManager->maximizeWindow((ServerWindow*) win);
}
