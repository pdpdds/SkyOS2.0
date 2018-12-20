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

#ifndef _WINDOW_MANAGER_H
#define _WINDOW_MANAGER_H

#include "Screen.h"

class ServerWindow;

/// The window manager processes mouse and keyboard events.  It:
///  - Handles focus and funnels keyboard events to the correct window
///  - Processes mouse movements and clicks.  Deliver to appropriate
///    windows and change focus as needed.
///  - Allow dragging and resizing windows
class WindowManager {
public:
	WindowManager(Screen *screen);
	void newWindow(ServerWindow *window);
	void closeWindow(ServerWindow *window);
	void doRepaint();

	void listWindows() const;

	void setRootWindow(ServerWindow*);

	void handleMouseMove(int x, int y);
	void handleMouseButton(int x, int y, int buttons);
	void handleKeyDown(int ch);
	void handleKeyUp(int ch);

	void lockMouseFocus();

	void maximizeWindow(ServerWindow *window);

	Screen *getScreen();

private:
	void setFocusedWindow(ServerWindow *window);

	Screen *fScreen;

	bool fEnableFocusFollowsMouse;
	ServerWindow *fRootWindow;
	ServerWindow *fFocusWindow;

	// Mouse state
	ServerWindow *fMouseWindow;		// Which window the mouse is over
	Rect fMouseRect;			// Rect that lies entirely within window
	bool fMouseFocusLocked;

	// Drag state
	ServerWindow *fDraggedWindow;
	int fGrabX;
	int fGrabY;

	// Resize state
	ServerWindow *fResizeWindow;
	bool fResizeLeft;
	bool fResizeBottom;
	bool fResizeRight;
	bool fResizeTop;
	int fOriginalWidth;
	int fOriginalHeight;
};


#endif
