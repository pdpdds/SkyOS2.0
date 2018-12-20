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

BlankWindow::BlankWindow(const Rect &frame, int color, Surface *bitmap)
	:	ServerWindow(frame, false),
		fColor(color),
		fBitmap(bitmap)
{
}

void BlankWindow::handleKeyDown(int ch)
{
	switch (ch) {
		case 82:
			fColor = 0xff0000;
			break;
		case 71:
			fColor = 0xff00;
			break;
		case 66:
			fColor = 0xff;
			break;
	}

	invalidate();
}

void BlankWindow::handleKeyUp(int ch)
{
}

void BlankWindow::paint(GraphicsContext &context)
{
	Rect &fr = frame();
	fr.offsetTo(0, 0);
	if (fBitmap != NULL) {
		context.blit(fBitmap, 0, 0, fBitmap->getWidth() - 1, fBitmap->getHeight() - 1,
			0, 0, fr.right, fr.bottom);
	} else {
		context.setColor(fColor);
		context.fillRect(fr.left, fr.top, fr.right, fr.bottom);
		context.setColor(0);
		int mx = fr.width() / 2;
		int my = fr.height() / 2;
		int size = my;

		for (int i = 0; i <= size; i += 10) {
			context.strokeLine(mx - i, my, mx, i);
			context.strokeLine(mx - i, my, mx, my + size - i);
			context.strokeLine(mx + i, my, mx, i);
			context.strokeLine(mx + i, my, mx, my + size - i);
		}
	}
}
