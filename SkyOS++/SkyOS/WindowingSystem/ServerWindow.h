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

/// @file Window.h
#ifndef __SERVER_WINDOW_H
#define __SERVER_WINDOW_H

#include "Atom.h"
#include "Rect.h"
#include "GraphicsContext.h"
#include "ServerConnection.h"
extern Region* kInfinite;
class ServerWindow : public Atom {
public:
	ServerWindow(const Rect &frame, bool doubleBuffer = false);

	// test only; these will go away when the client connection
	// is fully implemented.
	virtual void paint(GraphicsContext &context);
	virtual void handleMouseButton(int x, int y, int buttons);
	virtual void handleMouseMoved(int x, int y);
	virtual void handleMouseExit();
	virtual void handleKeyUp(int ch);
	virtual void handleKeyDown(int ch);

	void setServerConnection(ServerConnection*);

	void lockMouseFocus();
	void setDraggable(bool drag);

	ServerWindow *parent();

	/// Size
	void setResizeLimits(int minWidth, int maxWidth, int minHeight, int maxHeight);
	int getMinimumWidth() const;
	int getMaximumWidth() const;
	int getMinimumHeight() const;
	int getMaximumHeight() const;

	void maximize();
	void maximizeChild(ServerWindow *child);

	Region getVisibleRegion() const;

	/// @bug these two methods are need to be reconciled
	Rect frame() const;
	Region getBoundingRegion() const;

	/// Add a child window.  The child will be the topmost of all
	/// children
	void addChild(ServerWindow *window);
	void removeChild(ServerWindow *window);

	/// XXX only for frame windows
	/// @param window Pointer to a window that is an immediate child of this
	///   window.
	/// @returns
	///   - true if this actually moved something
	///   - false if the window was already in front
	bool moveToFront(ServerWindow *window);

	// Only for frame windows
	bool isFocused() const;
	void setFocused(bool focused);

	// Update the clipping regions of this window and all child windows,
	// passed in a visible region (clipped by this windows siblings or
	// an ancestors siblings)
	// the clip region is not constrained to the window's frame size
	// This will invalidate all sub windows as a side effect
	void updateClip(const Region &clipRegion);

	void updateClip();

	void invalidate(const Region &region = *kInfinite);

	void beginPaint();
	void endPaint();


	// Hack for now
	void doRepaint();

	GraphicsContext *getGC();

	/// Non recursive.  Just finds immediate child that contains
	/// this point.
	/// @param x Horizontal coordinate of point
	/// @param y Vertical coordinate of point.
	/// @returns
	///  - NULL if there was no child found at the specific point
	///  - NULL if the window found was a child of one of this window's
	///    children
	///  - Pointer to window if one was found
	ServerWindow *findChild(int x, int y);

	ServerWindow *findChildRecursive(int x, int y);

	// Hack for window management
	bool isDraggable() const;

	void moveChildBy(ServerWindow *child, int x, int y);
	void resizeChild(ServerWindow *child, int width, int height, bool rightSide,
		bool bottom);

protected:
	void setBoundingRegion(const Region &region);

private:
	virtual void resize(int x, int y, bool rightSide, bool bottom);
	void sendEvent(EventType type, int param1 = 0, int param2 = 0, int param3 = 0, int param4 = 0);

	// Used for faster scrolling and dragging.
	void copyRect(Rect src, int destX, int destY);
	void moveBy(long, long);

	void invalidateRecursive(const Region &region);

	void _addChild(ServerWindow *window);
	void _removeChild(ServerWindow *window);

	GraphicsContext *fGraphics;

	// Having both of these is redundant.  Need to reconcile
	Region fBounds;
	Rect fFrame;

	ServerWindow *fParent;
	ServerWindow *fChildList; ///< list of children, ordered topmost window first
	ServerWindow *fNextSibling; ///< Next pointer in my parent's child list

	Surface *fSurface;
	Region fGlobalClip; ///< Parts of this window not obscured by my siblings
	Region fLocalClip; ///< Global clip minus parts obscured by children
	Region fDirtyRegion; ///< Parts of this window that need to be repainted
	bool fIsFocused;
	int fMinimumWidth;
	int fMaximumWidth;
	int fMinimumHeight;
	int fMaximumHeight;

	// Repaint
	bool fInRepaint;
	GraphicsContext *fDoubleBufferContext;
	bool fEnableDoubleBufferedUpdates;
	Surface *fDoubleBufferSurface;
	Rect fDoubleBufferDest;

	bool fIsDraggable;

	// Maximize
	bool fIsMaximized;
	Rect fOldFrame;

	ServerConnection *fConnection;
};


#endif


