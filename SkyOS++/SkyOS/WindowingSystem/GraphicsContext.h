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

#ifndef _GRAPHICS_CONTEXT_H
#define _GRAPHICS_CONTEXT_H

#include "Rasterizer.h"
#include "Region.h"
#include "Surface.h"

/// Perhaps this should be split into a clipper and a container for graphics state.
class GraphicsContext {
public:
	GraphicsContext(Surface *surface, int xOrigin, int yOrigin);
	~GraphicsContext();
	void strokeLine(long x1, long y1, long x2, long y2);
	void strokeRect(long x1, long y1, long x2, long y2);
	void fillRect(long left, long top, long right, long bottom);
	void blit(Surface *source, int dLeft, int dTop);
	void blit(Surface *source, int sLeft, int sTop, int sRight, int sBottom,
		int dLeft, int dTop, int dRight, int dBottom);
	void setClipRegion(const Region &region);
	void setColor(int color);
	void setOrigin(int left, int top);
	void drawString(int x, int y, const char *string, int strlen);
	void setFont(const char *name, int nameLength);

	Region getClipRegion() const;
	void displayClipRegion();
	void pushState();
	void popState();

private:
	struct State {
		State *next;
		int currentColor;
	} *fStateStack;

	Region fClipRegion;
	Rasterizer *fRasterizer;
	Surface *fSurface;
	int fCurrentColor;
	int fXOrigin;
	int fYOrigin;
};


#endif
