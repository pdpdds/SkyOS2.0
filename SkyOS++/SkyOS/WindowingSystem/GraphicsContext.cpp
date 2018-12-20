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

#include "Rasterizer.h"
#include "Rect.h"
#include "Region.h"
#include "GraphicsContext.h"
#include "util.h"

#define swap(a, b) { long temp = a; a = b; b = temp; }

const unsigned kBottom = 1;
const unsigned kTop = 2;
const unsigned kLeft = 4;
const unsigned kRight = 8;

/// @bug this is inclusive-exclusive.  Everything else is inclusive-inclusive
inline int vert_clip(int x1, int y1, int x2, int y2, int x)
{
	return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
}

inline int horz_clip(int x1, int y1, int x2, int y2, int y)
{
	return x1 + (y - y1) * (x2 - x1) / (y2 - y1);
}

inline unsigned clipmask(int x, int y, int left, int top, int right, int bottom)
{
	unsigned mask = 0;

	if (x < left)
		mask |= kLeft;
	else if (x > right)
		mask |= kRight;

	if (y < top)
		mask |= kTop;
	else if (y > bottom)
		mask |= kBottom;

	return mask;
}


class LineClipper : public Clipper {
public:
	LineClipper(Rasterizer *rasterizer, int color, int x1, int y1, int x2, int y2)
		:	fRasterizer(rasterizer),
			fColor(color),
			fX1(x1),
			fY1(y1),
			fX2(x2),
			fY2(y2)
	{
	}

	virtual void operator()(int left, int top, int right, int bottom);

private:
	Rasterizer *fRasterizer;
	int fColor;
	int fX1;
	int fY1;
	int fX2;
	int fY2;
};


class RectClipper : public Clipper {
public:
	RectClipper(Rasterizer *rasterizer, int color, int x1, int y1, int x2, int y2)
		:	fRasterizer(rasterizer),
			fColor(color),
			fX1(x1),
			fY1(y1),
			fX2(x2),
			fY2(y2)
	{
		if (fX1 > fX2) swap(fX1, fX2);
		if (fY1 > fY2) swap(fY1, fY2);
	}

	virtual void operator()(int left, int top, int right, int bottom);

private:
	Rasterizer *fRasterizer;
	int fX1;
	int fY1;
	int fX2;
	int fY2;
	int fColor;
};

class BlitClipper : public Clipper {
public:
	BlitClipper(Surface *dest, Surface *source, int sourceLeft, int sourceTop, const Rect &destRect)
		:	fDestination(dest),
			fDestRect(destRect),
			fSource(source),
			fSourceTop(sourceTop),
			fSourceLeft(sourceLeft)
	{
	}

	virtual void operator()(int left, int top, int right, int bottom);

private:
	Surface *fDestination;
	Rect fDestRect;
	Surface *fSource;
	int fSourceTop;
	int fSourceLeft;
};

class StretchBlitClipper : public Clipper {
public:
	StretchBlitClipper(Surface *dest, Surface *source, const Rect &sourceRect, const Rect &destRect)
		:	fDestination(dest),
			fDestRect(destRect),
			fSource(source),
			fSourceRect(sourceRect)
	{
	}

	virtual void operator()(int left, int top, int right, int bottom);

private:
	Surface *fDestination;
	Rect fDestRect;
	Surface *fSource;
	Rect fSourceRect;
};

GraphicsContext::GraphicsContext(Surface *surface, int xOrigin, int yOrigin)
	:	fSurface(surface),
		fCurrentColor(0),
		fXOrigin(xOrigin),
		fYOrigin(yOrigin),
		fStateStack(0)
{
	fRasterizer = surface->createRasterizer();
}

GraphicsContext::~GraphicsContext()
{
	delete fRasterizer;
}


void GraphicsContext::setColor(int color)
{
	fCurrentColor = color;
}

void GraphicsContext::pushState()
{
	State *st = new State;
	st->currentColor = fCurrentColor;
	st->next = fStateStack;
}

void GraphicsContext::popState()
{
	State *st = fStateStack;
	fStateStack = fStateStack->next;

	fCurrentColor = st->currentColor;

	delete st;
}

void GraphicsContext::strokeLine(long x1, long y1, long x2, long y2)
{
	int globalX1 = x1 + fXOrigin;
	int globalY1 = y1 + fYOrigin;
	int globalX2 = x2 + fXOrigin;
	int globalY2 = y2 + fYOrigin;

	LineClipper clipper(fRasterizer, fCurrentColor, globalX1, globalY1,
		globalX2, globalY2);
	fClipRegion.clip(globalX1, globalY1, globalX2, globalY2, clipper);
}

void GraphicsContext::strokeRect(long x1, long y1, long x2, long y2)
{
	strokeLine(x1, y1, x2, y1);	// top
	strokeLine(x1, y1, x1, y2);	// left
	strokeLine(x2, y1, x2, y2);	// right
	strokeLine(x1, y2, x2, y2);	// bottom
}

void GraphicsContext::fillRect(long x1, long y1, long x2, long y2)
{
	int globalX1 = x1 + fXOrigin;
	int globalY1 = y1 + fYOrigin;
	int globalX2 = x2 + fXOrigin;
	int globalY2 = y2 + fYOrigin;

	RectClipper clipper(fRasterizer, fCurrentColor, globalX1, globalY1,
		globalX2, globalY2);
	fClipRegion.clip(globalX1, globalY1, globalX2, globalY2, clipper);
}

void GraphicsContext::blit(Surface *source, int dLeft, int dTop)
{
	blit(source, 0, 0, source->getWidth() - 1, source->getHeight() - 1,
		dLeft, dTop, dLeft + source->getWidth() - 1,
		dTop + source->getHeight() - 1);
}

void GraphicsContext::blit(Surface *source, int sLeft, int sTop, int sRight,
	int sBottom, int dLeft, int dTop, int dRight, int dBottom)
{
	Rect dest(dLeft, dTop, dRight, dBottom);
	dest.offsetBy(fXOrigin, fYOrigin);
	if (sRight - sLeft == dRight - dLeft && sBottom - sTop == dBottom - dTop) {
		// No stretching.(some of the parameters to the BlitClipper constructor
		// are redundant).
		BlitClipper clipper(fSurface, source, sLeft, sTop, dest);
		fClipRegion.clip(dest.left, dest.top, dest.right, dest.bottom, clipper);
	} else if (dest.right > dest.left && dest.bottom > dest.top) {
		StretchBlitClipper clipper(fSurface, source, Rect(sLeft, sTop, sRight,
			sBottom), dest);
		fClipRegion.clip(dest.left, dest.top, dest.right, dest.bottom, clipper);
	}
}

void GraphicsContext::drawString(int x, int y, const char *string, int strlen)
{
	// XXX not implemented
}

void GraphicsContext::setFont(const char *name, int nameLength)
{
	// XXX not implemented
}

void GraphicsContext::setClipRegion(const Region &region)
{
	fClipRegion = region;
}

void GraphicsContext::setOrigin(int left, int top)
{
	fXOrigin = left;
	fYOrigin = top;
}

Region GraphicsContext::getClipRegion() const
{
	return fClipRegion;
}

void GraphicsContext::displayClipRegion()
{
	displayRegion(fClipRegion, 0xdddddd);
}

void RectClipper::operator()(int left, int top, int right, int bottom)
{
	long clipleft = max(fX1, left);
	long cliptop = max(fY1, top);
	long clipright = min(fX2, right);
	long clipbottom = min(fY2, bottom);

	if (clipright >= clipleft && clipbottom >= cliptop) {
		fRasterizer->fillRect(clipleft, cliptop, clipright, clipbottom,
			fColor);
	}
}

//
// Cohen-Sutherland line clipper
//
void LineClipper::operator()(int left, int top, int right, int bottom)
{
	long clippedX1 = fX1;
	long clippedY1 = fY1;
	long clippedX2 = fX2;
	long clippedY2 = fY2;
	unsigned point1mask = clipmask(clippedX1, clippedY1, left, top, right, bottom);
	unsigned point2mask = clipmask(clippedX2, clippedY2, left, top, right, bottom);

	bool rejected = false;
	while (point1mask != 0 || point2mask != 0) {
		if ((point1mask & point2mask) != 0) {
			rejected = true;
			break;
		}

		unsigned  mask = point1mask ? point1mask : point2mask;
		long x, y;
		if (mask & kBottom) {
			y = bottom;
			x = horz_clip(clippedX1, clippedY1, clippedX2, clippedY2, y);
		} else if (mask & kTop) {
			y = top;
			x = horz_clip(clippedX1, clippedY1, clippedX2, clippedY2, y);
		} else if (mask & kRight) {
			x = right;
			y = vert_clip(clippedX1, clippedY1, clippedX2, clippedY2, x);
		} else if (mask & kLeft) {
			x = left;
			y = vert_clip(clippedX1, clippedY1, clippedX2, clippedY2, x);
		}

		if (point1mask) {
			// Clip point 1
			point1mask = clipmask(x, y, left, top, right, bottom);
			clippedX1 = x;
			clippedY1 = y;
		} else {
			// Clip point 2
			point2mask = clipmask(x, y, left, top, right, bottom);
			clippedX2 = x;
			clippedY2 = y;
		}
	}

	if (!rejected)
	 	fRasterizer->strokeLine(clippedX1, clippedY1, clippedX2, clippedY2, fColor);
}

void BlitClipper::operator()(int left, int top, int right, int bottom)
{
	int destLeft = max(fDestRect.left, left);
	int destTop = max(fDestRect.top, top);
	int destRight = min(fDestRect.right, right);
	int destBottom = min(fDestRect.bottom, bottom);
	int sourceLeft = fSourceLeft + (destLeft - fDestRect.left);
	int sourceTop = fSourceTop + (destTop - fDestRect.top);
	int sourceRight = sourceLeft + (destRight - destLeft);
	int sourceBottom = sourceTop + (destBottom - destTop);

	if (destRight >= destLeft && destBottom >= destTop) {
		fDestination->blit(fSource, sourceLeft, sourceTop, sourceRight,
			sourceBottom, destLeft, destTop, destRight, destBottom);
	}
}

/// @bug There is rounding error that will distort the image when only
/// part of it is drawn.
void StretchBlitClipper::operator()(int left, int top, int right, int bottom)
{
	int destLeft = max(fDestRect.left, left);
	int destTop = max(fDestRect.top, top);
	int destRight = min(fDestRect.right, right);
	int destBottom = min(fDestRect.bottom, bottom);
	if (destRight >= destLeft && destBottom >= destTop) {
		int sourceLeft = (destLeft - fDestRect.left)
			* fSourceRect.width() / fDestRect.width();
		int sourceRight = (destRight - fDestRect.left)
			 * fSourceRect.width() / fDestRect.width();
		int sourceTop = (destTop - fDestRect.top)
			* fSourceRect.height() / fDestRect.height();
		int sourceBottom = (destBottom - fDestRect.top)
			* fSourceRect.height() / fDestRect.height();

		fDestination->blit(fSource, sourceLeft, sourceTop, sourceRight,
			sourceBottom, destLeft, destTop, destRight,
			destBottom);
	}
}
