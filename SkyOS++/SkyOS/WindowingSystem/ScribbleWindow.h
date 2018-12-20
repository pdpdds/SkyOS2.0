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

#ifndef __SCRIBBLE_WINDOW_H
#define __SCRIBBLE_WINDOW_H

#include "ServerWindow.h"

class ScribbleWindow : public ServerWindow {
public:
	ScribbleWindow(Rect &bounds);
	~ScribbleWindow();
	virtual void paint(GraphicsContext &context);
	virtual void handleMouseButton(int x, int y, int buttons);
	virtual void handleMouseMoved(int x, int y);


private:
	Surface *fBackingStore;
	GraphicsContext *fBitmapContext;
	bool fButtonDown;
	int fLastX;
	int fLastY;
	int fSelectedColor;
};

#endif
