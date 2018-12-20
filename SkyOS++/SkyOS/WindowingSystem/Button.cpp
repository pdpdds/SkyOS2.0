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

#include "Button.h"

#define HIGHLIGHT 0xa0a0a0
#define NEUTRAL 0x808080
#define SHADOW 0x606060

Button::Button(const Rect &frame)
	:	ServerWindow(frame),
		fDisplayPressed(false),
		fMouseButtonDown(false),
		fCallback(0)
{
}

void Button::setCallback(void (*callback)(void *data), void *data)
{
	fCallback = callback;
	fCallbackData = data;
}

void Button::paint(GraphicsContext &gc)
{
	Rect bounds(frame());
	bounds.offsetTo(0, 0);

	gc.setColor(NEUTRAL);
	gc.fillRect(bounds.left, bounds.top, bounds.right, bounds.bottom);

	if (fDisplayPressed)
		gc.setColor(SHADOW);
	else
		gc.setColor(HIGHLIGHT);

	gc.strokeLine(bounds.left, bounds.bottom, bounds.left, bounds.top);
	gc.strokeLine(bounds.left, bounds.top, bounds.right, bounds.top);
	if (fDisplayPressed)
		gc.setColor(HIGHLIGHT);
	else
		gc.setColor(SHADOW);

	gc.strokeLine(bounds.right, bounds.top, bounds.right, bounds.bottom);
	gc.strokeLine(bounds.right, bounds.bottom, bounds.left, bounds.bottom);
}

void Button::handleMouseButton(int x, int y, int buttons)
{
	if (buttons & 1) {
		if (!fMouseButtonDown) {
			lockMouseFocus();
			fMouseButtonDown = true;
			fDisplayPressed = true;
			invalidate();
		}
	} else {
		if (fMouseButtonDown) {
			fMouseButtonDown = false;
			if (fDisplayPressed) {
				fDisplayPressed = false;
				invalidate();

				if (fCallback)
					(*fCallback)(fCallbackData);
			}
		}
	}
}

void Button::handleMouseMoved(int x, int y)
{
	if (frame().contains(x, y)) {
		if (!fDisplayPressed && fMouseButtonDown) {
			fDisplayPressed = true;
			invalidate();
		}
	} else {
		if (fDisplayPressed) {
			fDisplayPressed = false;
			invalidate();
		}
	}
}
