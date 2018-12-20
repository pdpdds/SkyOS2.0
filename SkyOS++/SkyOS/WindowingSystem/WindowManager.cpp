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

#include "debug.h"
#include "ServerWindow.h"
#include "WindowManager.h"
#include "Region.h"

WindowManager::WindowManager(Screen *screen)
	:	fEnableFocusFollowsMouse(true),
		fMouseFocusLocked(false),
		fScreen(screen),
		fFocusWindow(0),
		fRootWindow(0),
		fDraggedWindow(0),
		fResizeWindow(0),
		fMouseWindow(0)
{
}

void WindowManager::setRootWindow(ServerWindow *root)
{
	fRootWindow = root;
	fRootWindow->updateClip(*kInfinite);
}

void WindowManager::doRepaint()
{
	fRootWindow->doRepaint();
}

void WindowManager::newWindow(ServerWindow *window)
{
	fRootWindow->addChild(window);
	if (fFocusWindow)
		fFocusWindow->setFocused(false);

	fFocusWindow = window;
	fFocusWindow->setFocused(true);
}

void WindowManager::closeWindow(ServerWindow *window)
{
	fRootWindow->removeChild(window);

	// @bug Subwindows will be leaked.  Fix this
	delete window;

	fRootWindow->updateClip();

	// XXX this may be a little disruptive, but we need to know if the window
	// pointed to by any of these is a descendent of the closed window.
	// Perhaps this could be beefed up in the future.
	fMouseWindow = 0;
	fResizeWindow = 0;
	fDraggedWindow = 0;

	// XXX This also has the side effect that the mouse won't click until you move it.

	if (fFocusWindow == window)
		fFocusWindow = fRootWindow;

	fMouseFocusLocked = false;
}

void WindowManager::maximizeWindow(ServerWindow *window)
{
	setFocusedWindow(window);


	fRootWindow->maximizeChild(window);
}

void WindowManager::listWindows() const
{
}

Screen *WindowManager::getScreen()
{
	return fScreen;
}

void WindowManager::handleMouseMove(int x, int y)
{
	if (fDraggedWindow) {
		// Drag the window around
		fRootWindow->moveChildBy(fDraggedWindow, x - fGrabX, y - fGrabY);
		fGrabX = x;
		fGrabY = y;
	} else if (fResizeWindow) {
		int newHeight;
		if (fResizeBottom)
			newHeight = y - fGrabY + fOriginalHeight;
		else if (fResizeTop)
			newHeight = fGrabY - y + fOriginalHeight;
		else
			newHeight = fOriginalHeight;

		int newWidth;
		if (fResizeRight)
			newWidth = x - fGrabX + fOriginalWidth;
		else if (fResizeLeft)
			newWidth = fGrabX - x + fOriginalWidth;
		else
			newWidth = fOriginalWidth;

		if (newWidth > fResizeWindow->getMaximumWidth())
			newWidth = fResizeWindow->getMaximumWidth();

		if (newWidth < fResizeWindow->getMinimumWidth())
			newWidth = fResizeWindow->getMinimumWidth();

		if (newHeight > fResizeWindow->getMaximumHeight())
			newHeight = fResizeWindow->getMaximumHeight();

		if (newHeight < fResizeWindow->getMinimumHeight())
			newHeight = fResizeWindow->getMinimumHeight();

		fRootWindow->resizeChild(fResizeWindow, newWidth, newHeight, fResizeRight,
			fResizeBottom);
	} else {
		// Determine which window we are in
		ServerWindow *newWindow = fMouseWindow;
		if (!fMouseFocusLocked) {
			if (newWindow == 0) {
				newWindow = fRootWindow->findChildRecursive(x, y);
				fMouseRect = newWindow->getVisibleRegion().getBoundingRect(x, y);
			} else {
				// Try to be smart about finding the current window
				// Instead of just doing an expensive recursive lookup every time,
				// detect if we've moved out of the current window.
				if (!fMouseRect.contains(x, y)) {
					// We've moved out of the rectangle, check if we're still in the window
					if (!fMouseWindow->getVisibleRegion().contains(x, y)) {
						// Nope, do a lookup to see where we've moved to.
						newWindow = fRootWindow->findChildRecursive(x, y);
						fMouseRect = newWindow->getVisibleRegion().getBoundingRect(x, y);
					}
				}
			}
		}

		if (newWindow == fMouseWindow) {
			// Post a mouse-moved event
			fMouseWindow->handleMouseMoved(x, y);
		} else {
			// The mouse has now entered a new window
			if (fMouseWindow) {
				fMouseWindow->handleMouseExit();
			}

			newWindow->handleMouseMoved(x, y);
			fMouseWindow = newWindow;
			if (fEnableFocusFollowsMouse) {
				ServerWindow *newFocusWindow = fRootWindow->findChild(x, y);
				if (newFocusWindow != fFocusWindow)
					setFocusedWindow(newFocusWindow);
			}
		}
	}
}

void WindowManager::setFocusedWindow(ServerWindow *window)
{
	if (window != fFocusWindow) {
		fFocusWindow->setFocused(false);
		fFocusWindow->invalidate();	// Redraw title bar
		fFocusWindow = window;
		fFocusWindow->setFocused(true);
		fFocusWindow->invalidate();
	}
}

void WindowManager::handleMouseButton(int x, int y, int buttons)
{
	if (buttons & 1) {
		if (fMouseWindow != 0) {
			if (fMouseWindow->isDraggable()) {
				// Check if this is actually on the frame.  If so, we can drag or resize
				// the window.  If not, just bring the window to the front.
				if (x < fMouseWindow->frame().left + 10) {
					fResizeLeft = true;
					fResizeRight = false;
				} else if (x > fMouseWindow->frame().right - 10) {
					fResizeLeft = false;
					fResizeRight = true;
				} else {
					fResizeLeft = false;
					fResizeRight = false;
				}

				if (y < fMouseWindow->frame().top + 3) {
					fResizeTop = true;
					fResizeBottom = false;
				} else if (y > fMouseWindow->frame().bottom - 10) {
					fResizeTop = false;
					fResizeBottom = true;
				} else {
					fResizeTop = false;
					fResizeBottom = false;
				}

				if (fResizeLeft || fResizeBottom || fResizeRight || fResizeTop) {
					// Begin resizing this window
					fResizeWindow = fMouseWindow;
					fOriginalWidth = fResizeWindow->frame().width();
					fOriginalHeight = fResizeWindow->frame().height();
				} else {
					// This is a drag event
					fDraggedWindow = fMouseWindow;
				}

				fGrabX = x;
				fGrabY = y;
			}

			ServerWindow *newFocusWindow = fRootWindow->findChild(x, y);
			setFocusedWindow(newFocusWindow);
			if (newFocusWindow != fRootWindow)
				fRootWindow->moveToFront(newFocusWindow);
		}
	} else {
		fDraggedWindow = 0;
		fResizeWindow = 0;
	}

	// Just deliver this event to the window
	if (fMouseWindow != 0) {
		fMouseWindow->handleMouseButton(x, y, buttons);

		if (fMouseFocusLocked && buttons == 0) {
			fMouseFocusLocked = false;

			// Fake out a mouse movement to fix the focus and send an event to the new window
			handleMouseMove(x, y);
		}
	}
}

void WindowManager::handleKeyUp(int ch)
{
	fFocusWindow->handleKeyUp(ch);
}

void WindowManager::handleKeyDown(int ch)
{
	fFocusWindow->handleKeyDown(ch);
}

void WindowManager::lockMouseFocus()
{
	fMouseFocusLocked = true;
}
