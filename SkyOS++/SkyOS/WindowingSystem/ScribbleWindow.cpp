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

///
/// @note this will be moved to client side code.
///
#include "ScribbleWindow.h"
#include "MemorySurface.h"

#define PALETTE_HEIGHT 16

const static int kColorMap[] = { 0, 0xff, 0xff00, 0xff0000, 0xffff, 0xffff00, 0xff00ff, 0xffffff };

ScribbleWindow::ScribbleWindow(Rect &bounds)
	:	ServerWindow(bounds),
		fButtonDown(false),
		fSelectedColor(7)
{
	setResizeLimits(bounds.width(), bounds.width(), bounds.height(), bounds.height());
	fBackingStore = new MemorySurface(Surface::kRGB24, bounds.width(), bounds.height() - PALETTE_HEIGHT);
	fBitmapContext = new GraphicsContext(fBackingStore, 0, 0);

	fBitmapContext->setClipRegion(Region(0, 0, bounds.width() - 1, bounds.height() - PALETTE_HEIGHT - 1));
	fBitmapContext->setColor(0);
	fBitmapContext->fillRect(0, 0, bounds.width(), bounds.height());
	fBitmapContext->setColor(0xffffff);
}

ScribbleWindow::~ScribbleWindow()
{
	delete fBackingStore;
	delete fBitmapContext;
}

void ScribbleWindow::paint(GraphicsContext &gc)
{
	gc.blit(fBackingStore, 0, 0);

	gc.setColor(0x202020);
	gc.fillRect(0, frame().height() - PALETTE_HEIGHT, frame().width(), frame().height());

	// Draw the palette
	for (int i = 0; i < 8; i++) {
		Rect colorSplotchFrame(i * frame().width() / 8, frame().height() - PALETTE_HEIGHT, (i + 1) * frame().width() / 8,
			frame().height());

		if (fSelectedColor == i) {
			gc.setColor(0x808080);
			gc.fillRect(colorSplotchFrame.left, colorSplotchFrame.top, colorSplotchFrame.right, colorSplotchFrame.bottom);
		}

		colorSplotchFrame.insetBy(3, 3);
		gc.setColor(kColorMap[i]);
		gc.fillRect(colorSplotchFrame.left, colorSplotchFrame.top, colorSplotchFrame.right, colorSplotchFrame.bottom);
	}
}

void ScribbleWindow::handleMouseButton(int x, int y, int buttons)
{
	if (buttons & 1) {
		if (y > frame().bottom - PALETTE_HEIGHT) {
			// Select a new color
			int newColor = (x - frame().left) * 8 / frame().width();
			if (newColor != fSelectedColor) {
				fSelectedColor = newColor;
				invalidate();
			}
		} else {
			fButtonDown = true;
			lockMouseFocus();
			fLastX = x - frame().left;
			fLastY = y - frame().top;
		}
	} else {
		fButtonDown = false;
	}
}

void ScribbleWindow::handleMouseMoved(int x, int y)
{
	if (fButtonDown && frame().contains(x, y)) {
		fBitmapContext->setColor(kColorMap[fSelectedColor]);
		fBitmapContext->strokeLine(fLastX, fLastY, x - frame().left, y
			- frame().top);
		invalidate();
	}

	fLastX = x - frame().left;
	fLastY = y - frame().top;
}

