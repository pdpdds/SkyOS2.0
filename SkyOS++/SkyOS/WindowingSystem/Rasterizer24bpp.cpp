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

#include "Rasterizer24bpp.h"
#include "debug.h"

Rasterizer24bpp::Rasterizer24bpp(Surface *surface)
	:	Rasterizer(surface)
{
}

void Rasterizer24bpp::strokeLine(int x1, int y1, int x2, int y2, int color)
{
	Surface *surface = getSurface();
	char *base = (char*) surface->lockBits();
	int stride = surface->getStride();

	assert(x1 >= 0 && x1 < surface->getWidth());
	assert(x2 >= 0 && x2 < surface->getWidth());
	assert(y1 >= 0 && y1 < surface->getHeight());
	assert(y2 >= 0 && y2 < surface->getHeight());

	// Swap if necessary so we always draw top to bottom
	if (y1 > y2) {
		int temp = y1;
		y1 = y2;
		y2 = temp;

		temp = x1;
		x1 = x2;
		x2 = temp;
	}

	int deltaY = (y2 - y1) + 1;
	int deltaX = x2 > x1 ? (x2 - x1) + 1 : (x1 - x2) + 1;
	int xDir = x2 > x1 ? 1 : -1;
	int error = 0;
	char *ptr = base + (x1 * 3) + y1 * stride;

	if (deltaX == 0) {
		// Vertical line
		for (int y = deltaY; y > 0; y--) {
			ptr[0] = color & 0xff;
			ptr[1] = (color >> 8) & 0xff;
			ptr[2] = (color >> 16) & 0xff;
			ptr += stride;
		}
	} else if (deltaY == 0) {
		// Horizontal line
		for (int x = deltaX; x > 0; x--) {
			ptr[0] = color & 0xff;
			ptr[1] = (color >> 8) & 0xff;
			ptr[2] = (color >> 16) & 0xff;
			ptr += xDir * 3;
		}
	} else if (deltaX > deltaY) {
		// Diagonal with horizontal major axis
		int x = x1;
		for (;;) {
			ptr[0] = color & 0xff;
			ptr[1] = (color >> 8) & 0xff;
			ptr[2] = (color >> 16) & 0xff;

			error += deltaY;
			if (error > deltaX) {
				ptr += stride;
				error -= deltaX;
			}

			ptr += xDir * 3;
			if (x == x2)
				break;

			x += xDir;
		}
	} else {
		// Diagonal with vertical major axis
		for (int y = y1; y <= y2; y++) {
			ptr[0] = color & 0xff;
			ptr[1] = (color >> 8) & 0xff;
			ptr[2] = (color >> 16) & 0xff;

			error += deltaX;
			if (error > deltaY) {
				ptr += xDir * 3;
				error -= deltaY;
			}

			ptr += stride;
		}
	}

	surface->unlockBits();
}

void Rasterizer24bpp::fillRect(int x1, int y1, int x2, int y2, int color)
{
	Surface *surface = getSurface();

	assert(x1 >= 0 && x1 <= surface->getWidth());
	assert(x2 >= 0 && x2 <= surface->getWidth());
	assert(y1 >= 0 && y1 <= surface->getHeight());
	assert(y2 >= 0 && y2 <= surface->getHeight());
	assert (x2 >= x1);
	assert (y2 >= y1);

	char *base = (char*) surface->lockBits();
	int stride = surface->getStride();
	char *ptr = base + (x1 * 3) + y1 * stride;

	for (int y = y1; y <= y2; y++) {
		for (int x = x1; x <= x2; x++) {
			ptr[0] = color & 0xff;
			ptr[1] = (color >> 8) & 0xff;
			ptr[2] = (color >> 16) & 0xff;
			ptr += 3;
		}

		ptr += stride - ((x2 - x1) + 1) * 3;
	}

	surface->unlockBits();
}
