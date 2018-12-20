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
#include "RootWindow.h"

RootWindow::RootWindow(long width, long height)
	:	ServerWindow(Rect(0, 0, width, height)),
		fSelecting(false)
{
}

void RootWindow::paint(GraphicsContext &context)
{
	context.setColor(0x000080);
	context.fillRect(0, 0, frame().width(), frame().height());
	if (fSelecting) {
		context.setColor(0);
		context.strokeRect(fSelectX1, fSelectY1, fSelectX2, fSelectY2);
	}
}

void RootWindow::handleMouseButton(int x, int y, int buttons)
{
	if (buttons & 1) {
		lockMouseFocus();
		fSelectX1 = x;
		fSelectY1 = y;
		fSelectX2 = x;
		fSelectY2 = y;
		fSelecting = true;
		invalidate();
	} else {
		fSelecting = false;
		invalidate();
	}
}

void RootWindow::handleMouseMoved(int x, int y)
{
	if (fSelecting) {
		fSelectX2 = x;
		fSelectY2 = y;
		invalidate();
	}
}
