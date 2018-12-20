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
#include "MemorySurface.h"
#include "Rect.h"
#include "ServerWindow.h"
#include "WindowManager.h"

#define FAST_WINDOW_DRAG 0

class CopyRectClipper : public Clipper {
public:
	CopyRectClipper(Surface *surface, Rect sourceRect, int destX, int destY)
		:	fSurface(surface),
			fSourceRect(sourceRect),
			fDX(sourceRect.left - destX),	// distance from dest to source
			fDY(sourceRect.top - destY)		// distance from dest to source
	{
	}

	virtual void operator()(int left, int top, int right, int bottom);

private:
	Surface *fSurface;
	Rect fSourceRect;
	int fDX;
	int fDY;
};

extern WindowManager *windowManager;

ServerWindow::ServerWindow(const Rect& rect, bool doubleBuffer)
	:	Atom(ATOM_SERVER_WINDOW),
		fFrame(rect),
		fChildList(0),
		fNextSibling(0),
		fIsFocused(false),
		fBounds(rect.left, rect.top, rect.right, rect.bottom),
		fMaximumWidth(500),
		fMinimumWidth(50),
		fMaximumHeight(400),
		fMinimumHeight(50),
		fEnableDoubleBufferedUpdates(doubleBuffer),
		fIsMaximized(false),
		fIsDraggable(false),
		fInRepaint(false),
		fDoubleBufferContext(NULL),
		fConnection(NULL)
{
	fSurface = windowManager->getScreen();
	fGraphics =  new GraphicsContext(fSurface, rect.left, rect.top);
}

// This assumes a rectangular window
Rect ServerWindow::frame() const
{
	return fFrame;
}

bool ServerWindow::moveToFront(ServerWindow *window)
{
	if (fChildList == window)
		return false;

	_removeChild(window);
	_addChild(window);
	updateClip();
	return true;
}

void ServerWindow::lockMouseFocus()
{
	windowManager->lockMouseFocus();
}

bool ServerWindow::isFocused() const
{
	return fIsFocused;
}

void ServerWindow::setFocused(bool focused)
{
	if (focused)
		sendEvent(EVT_GOT_FOCUS);
	else
		sendEvent(EVT_LOST_FOCUS);

	fIsFocused = focused;
}

void ServerWindow::addChild(ServerWindow *window)
{
	assert(fSurface != NULL);
	_addChild(window);
	window->fParent = this;
	updateClip();
}

void ServerWindow::removeChild(ServerWindow *window)
{
	_removeChild(window);
	updateClip();
}

ServerWindow *ServerWindow::parent()
{
	return fParent;
}

void ServerWindow::_addChild(ServerWindow *window)
{
	assert(window != this);

	window->fNextSibling = fChildList;
	fChildList = window;
}

void ServerWindow::_removeChild(ServerWindow *window)
{
	assert(window != this);

	if (fChildList == window)
		fChildList = fChildList->fNextSibling;
	else {
		bool found = false;
		for (ServerWindow *parent = fChildList; parent != NULL; parent = parent->fNextSibling) {
			if (parent->fNextSibling == window) {
				parent->fNextSibling = parent->fNextSibling->fNextSibling;
				found = true;
				break;
			}
		}

		assert(found);
	}
}

void ServerWindow::updateClip()
{
	updateClip(fGlobalClip);
}

void ServerWindow::updateClip(const Region &visibleRegion)
{
	Region oldVisibleRegion = fLocalClip;
	fGlobalClip = visibleRegion & getBoundingRegion();
	fLocalClip = fGlobalClip;

	// Walk through children, clipping them against each other and clipping
	// this window against all of them.
	for (ServerWindow *child = fChildList; child != NULL; child = child->fNextSibling) {
		child->updateClip(fLocalClip);
		fLocalClip -= child->getBoundingRegion();
	}

	// Determine exposed region and invalidate
	invalidate(fLocalClip - oldVisibleRegion);
	fGraphics->setClipRegion(fLocalClip);
	fGraphics->setOrigin(fFrame.left, fFrame.top);
}

void ServerWindow::moveBy(long x, long y)
{
	fFrame.offsetBy(x, y);
	fBounds.translateBy(x, y);

	for (ServerWindow *child = fChildList; child != NULL; child = child->fNextSibling) {
		child->moveBy(x, y);
	}

	invalidate();
}

void ServerWindow::moveChildBy(ServerWindow *child, int x, int y)
{
	child->moveBy(x, y);
	updateClip();
}

void ServerWindow::resizeChild(ServerWindow *child, int width, int height, bool rightSide,
	bool bottom)
{
	child->resize(width, height, rightSide, bottom);
	updateClip();
}

void ServerWindow::maximize()
{
	int dX;
	int dY;
	int dWidth;
	int dHeight;

	if (fIsMaximized) {
		dX = fOldFrame.left;
		dY = fOldFrame.top;
		dWidth = fOldFrame.width() - fFrame.width();
		dHeight = fOldFrame.height() - fFrame.height();

		fFrame = fOldFrame;
	} else {
		fOldFrame = fFrame;

		dX = -fFrame.left;
		dY = -fFrame.top;
		dWidth = fSurface->getWidth() - fFrame.width();
		dHeight = fSurface->getHeight() - fFrame.height();

		fFrame.left = 0;
		fFrame.top = 0;
		fFrame.right = fSurface->getWidth() - 1;
		fFrame.bottom = fSurface->getHeight() - 1;
	}

	fIsMaximized = !fIsMaximized;

	fBounds = Region(fFrame.left, fFrame.top, fFrame.right, fFrame.bottom);

	sendEvent(EVT_RESIZED, fFrame.width(), fFrame.height());

	invalidate();

	for (ServerWindow *child = fChildList; child != NULL; child = child->fNextSibling)
		child->moveBy(dX, dY);

	// XXX hack
	if (fIsDraggable && fChildList != 0) {
		int oldChildWidth = fChildList->frame().width();
		int oldChildHeight = fChildList->frame().height();
		fChildList->resize(oldChildWidth + dWidth, oldChildHeight + dHeight,
			true, true);
	}
}

void ServerWindow::maximizeChild(ServerWindow *child)
{
	child->maximize();
	updateClip();
}

Region ServerWindow::getBoundingRegion() const
{
	return fBounds;
}

Region ServerWindow::getVisibleRegion() const
{
	return fLocalClip;
}

void ServerWindow::invalidate(const Region &region)
{
	bool wasDirty = !fDirtyRegion.isEmpty();

	fDirtyRegion |= region; // XXX should this be clipped against my frame?
	if (!wasDirty && !fDirtyRegion.isEmpty())
		sendEvent(EVT_PAINT);
}

/// XXX hack
void ServerWindow::doRepaint()
{
	if (fConnection != 0)
		return;	// This has a client attached, skip this internal stuff.

	for (ServerWindow *child = fChildList; child != NULL; child = child->fNextSibling)
		child->doRepaint();

	if (!fDirtyRegion.isEmpty()) {
		beginPaint();
		paint(*getGC());
		endPaint();
	}
}

void ServerWindow::beginPaint()
{
	if (fEnableDoubleBufferedUpdates) {
		// When the user selects automatic double buffered updates, all drawing will occur to a bitmap,
		// which will then be blitted to the window in finished form.  This prevents flicker.
		Region redrawRegion = fDirtyRegion & fLocalClip;
		fDoubleBufferDest = redrawRegion.getBounds();
		fDoubleBufferSurface = new MemorySurface(Surface::kRGB24, fDoubleBufferDest.width(), fDoubleBufferDest.height());
		fDoubleBufferContext = new GraphicsContext(fDoubleBufferSurface, frame().left - fDoubleBufferDest.left,
			frame().top - fDoubleBufferDest.top);
		fDoubleBufferContext->setClipRegion(Region(0, 0, fDoubleBufferDest.width() - 1,
			fDoubleBufferDest.height() - 1));
	} else {
		fGraphics->setClipRegion(fDirtyRegion & fLocalClip);
		fDirtyRegion = Region::kNull;
	}
}

void ServerWindow::endPaint()
{
	if (fEnableDoubleBufferedUpdates) {
		delete fDoubleBufferContext;
		fDoubleBufferContext = NULL;
		fGraphics->blit(fDoubleBufferSurface, fDoubleBufferDest.left, fDoubleBufferDest.top);
		delete fDoubleBufferSurface;
		fDoubleBufferSurface = NULL;
	} else
		fGraphics->setClipRegion(fLocalClip);
}

void ServerWindow::paint(GraphicsContext &context)
{
}

GraphicsContext *ServerWindow::getGC()
{
	if (fDoubleBufferContext != NULL)
		return fDoubleBufferContext;

	return fGraphics;
}

ServerWindow *ServerWindow::findChild(int x, int y)
{
	for (ServerWindow *child = fChildList; child != NULL; child = child->fNextSibling) {
		if (child->getBoundingRegion().contains(x, y))
			return child;
	}

	return this;
}

ServerWindow *ServerWindow::findChildRecursive(int x, int y)
{
	for (ServerWindow *child = fChildList; child != NULL; child = child->fNextSibling) {
		if (child->getBoundingRegion().contains(x, y))
			return child->findChildRecursive(x, y);
	}

	return this;
}

void ServerWindow::setDraggable(bool drag)
{
	fIsDraggable = drag;
}

bool ServerWindow::isDraggable() const
{
	return fIsDraggable && !fIsMaximized;
}

void ServerWindow::handleMouseButton(int x, int y, int buttons)
{
	sendEvent(EVT_MOUSE_BUTTON, 0, x - fFrame.left, y - fFrame.top, buttons);
}

void ServerWindow::handleMouseMoved(int x, int y)
{
	sendEvent(EVT_MOUSE_MOVED, 0, x - fFrame.left, y - fFrame.top, 0);
}

void ServerWindow::handleKeyDown(int ch)
{
	// XXX hack
	if (fIsDraggable)
		fChildList->handleKeyDown(ch);
	else
		sendEvent(EVT_KEY_DOWN, ch);
}

void ServerWindow::handleKeyUp(int ch)
{
	// XXX hack
	if (fIsDraggable)
		fChildList->handleKeyUp(ch);
	else
		sendEvent(EVT_KEY_UP, ch);
}

void ServerWindow::handleMouseExit()
{
	sendEvent(EVT_MOUSE_EXIT);
}

void ServerWindow::sendEvent(EventType type, int param1, int param2, int param3, int param4)
{
	if (fConnection != NULL)
		fConnection->sendEvent(type, getID(), param1, param2, param3, param4);
}

void ServerWindow::setServerConnection(ServerConnection *connection)
{
	fConnection = connection;
}

void ServerWindow::copyRect(Rect src, int destX, int destY)
{
	// A copyable block is one that is currently visible
	// and will be visible in its new position.
	// The copyable region is expressed in terms of the
	// destination region
	Region copyable(fGlobalClip);
	copyable.translateBy(destX - src.left, destY - src.top);
	copyable &= fGlobalClip;

	Region temp;
	temp |= Region(src.left, src.top, src.right, src.bottom);
	temp |= Region(destX, destY, destX + src.width(), destY + src.height());

	copyable &= temp;

	// Copy regions that are visible
	CopyRectClipper clipper(fSurface, src, destX, destY);
	copyable.clip(destX, destY, destX + src.width(), destY + src.height(), clipper);

	// Invalidate areas that couldn't be copied because the source
	// wasn't visible but the destination was.
	invalidateRecursive(fGlobalClip - copyable);
}

int ServerWindow::getMinimumWidth() const
{
	return fMinimumWidth;
}

int ServerWindow::getMaximumWidth() const
{
	return fMaximumWidth;
}

int ServerWindow::getMinimumHeight() const
{
	return fMinimumHeight;
}

int ServerWindow::getMaximumHeight() const
{
	return fMaximumHeight;
}

void ServerWindow::setResizeLimits(int minWidth, int maxWidth, int minHeight, int maxHeight)
{
	fMinimumWidth = minWidth;
	fMaximumWidth = maxWidth;
	fMinimumHeight = minHeight;
	fMaximumHeight = maxHeight;
}

void ServerWindow::invalidateRecursive(const Region &region)
{
	for (ServerWindow *child = fChildList; child != NULL; child = child->fNextSibling) {
		invalidate(region);
		child->invalidateRecursive(region);
	}
}

void ServerWindow::resize(int width, int height, bool rightSide, bool bottom)
{
	int dWidth = width - fFrame.width();
	int dHeight = height - fFrame.height();

	if (rightSide)
		fFrame.right += dWidth;
	else
		fFrame.left -= dWidth;

	if (bottom)
		fFrame.bottom += dHeight;
	else
		fFrame.top -= dHeight;

	fBounds = Region(fFrame.left, fFrame.top, fFrame.right, fFrame.bottom);

	sendEvent(EVT_RESIZED, 0, width, height);
	invalidate();

	if (!rightSide || !bottom) {
		int dX = rightSide ? 0 : -dWidth;
		int dY = bottom ? 0 : -dHeight;
		for (ServerWindow *child = fChildList; child != NULL; child = child->fNextSibling)
			child->moveBy(dX, dY);
	}

	// XXX hack
	if (fIsDraggable && fChildList != 0) {
		int oldChildWidth = fChildList->frame().width();
		int oldChildHeight = fChildList->frame().height();
		fChildList->resize(oldChildWidth + dWidth, oldChildHeight + dHeight,
			true, true);
	}
}

void ServerWindow::setBoundingRegion(const Region &region)
{
	fBounds = region;
}

void CopyRectClipper::operator()(int left, int top, int right, int bottom)
{
	fSurface->blit(fSurface, left + fDX, top + fDY, right + fDX, bottom + fDY,
		left, top, right, bottom);
}
