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

#ifndef ROOTWINDOW_H
#define ROOTWINDOW_H

#include "ServerWindow.h"

class RootWindow : public ServerWindow {
public:
	RootWindow(long screenWidth, long screenHeight);
	virtual void paint(GraphicsContext&);

	virtual void handleMouseButton(int x, int y, int buttons);
	virtual void handleMouseMoved(int x, int y);

public:
	bool fSelecting;
	int fSelectX1;
	int fSelectY1;
	int fSelectX2;
	int fSelectY2;
};

#endif
