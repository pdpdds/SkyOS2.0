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

#include <string.h>
#include "debug.h"
#include "Surface.h"
#include "memory.h"

Surface::Surface(ColorSpace colorSpace, int width, int height)
	:	fColorSpace(colorSpace),
		fWidth(width),
		fHeight(height)
{
}

Surface::~Surface()
{
}

Surface::ColorSpace Surface::getColorSpace() const
{
	return fColorSpace;
}

int Surface::getWidth() const
{
	return fWidth;
}

int Surface::getHeight() const
{
	return fHeight;
}

int Surface::getBitDepth() const
{
	switch (fColorSpace) {
		case kCMAP8:
			return 8;

		case kRGB565:
			return 16;

		case kRGB24:
			return 24;

		case kRGBA32:
			return 32;
	}

	return 0;
}


// The default implementation of blit simply copies directly from one
// memory area to another.  If multiple surfaces come from the same pool,
// they can possibly use an accelerated blit.  Those surfaces must override
// blit and determine if the source is a compatible surface.  If it is not,
// they can always call the default version.
// Note that the source surface might actually be this object
void Surface::blit(Surface *source, int sLeft, int sTop, int sRight, int sBottom,
	int dLeft, int dTop, int dRight, int dBottom)
{
	// XXX check colorspace compatiblity
	if (source->getColorSpace() != getColorSpace())
		return;	// Not supported

	// Fix all of the arguments
	if (dLeft < 0)
		dLeft = 0;

	if (dRight >= getWidth())
		dRight = getWidth() - 1;

	if (dBottom >= getHeight())
		dBottom = getHeight() - 1;

	if (dTop < 0)
		dTop = 0;

	if (sLeft < 0)
		sLeft = 0;

	if (sRight >= source->getWidth())
		sRight = source->getWidth() - 1;

	if (sBottom >= source->getHeight())
		sBottom = source->getHeight() - 1;

	if (sTop < 0)
		sTop = 0;

	if (source == this) {
		int lineSize = (sRight - sLeft) * 3;
		if (dTop > sTop && dTop < sBottom) {
			// Vertical overlap, copy from bottom to top
			char *srcptr = (char*) source->lockBits() + (sLeft  * 3) + sBottom * getStride();
			char *destptr = srcptr + ((dLeft - sLeft) * 3 + (dTop - sTop) * getStride());
			int stride = getStride();
			for (int y = sTop; y <= sBottom; y++) {
				memmove(destptr, srcptr, lineSize);
				srcptr -= stride;
				destptr -= stride;
			}

			unlockBits();
		} else {
			// Copy from top to bottom
			char *srcptr = (char*) source->lockBits() + (sLeft  * 3) + sTop * getStride();
			char *destptr = srcptr + ((dLeft - sLeft) * 3 + (dTop - sTop) * getStride());
			int stride = getStride();
			for (int y = sTop; y <= sBottom; y++) {
				memmove(destptr, srcptr, lineSize);
				srcptr += stride;
				destptr += stride;
			}

			unlockBits();
		}
	} else if (dRight - dLeft == sRight - sLeft
		&& dBottom - dTop == sBottom - sTop) {
		// Equally sized regions (note that you could make a few more
		// fast paths here)
		char *sourceAddr = (char*) source->lockBits() + (sTop * source->getStride()) + (sLeft * 3);
		char *destAddr = (char*) lockBits() + (dTop * getStride()) + (dLeft * 3);
		for (int y = dTop; y <= dBottom; y++) {
			memcpy(destAddr, sourceAddr, (dRight - dLeft + 1) * 3);
			destAddr += getStride();
			sourceAddr += source->getStride();
		}

		unlockBits();
		source->unlockBits();
	} else {
		// Need to stretch/shrink
		int verror = 0;
		char *destPtr = (char*) lockBits() + (dLeft * 3) + dTop
			* getStride();
		char *sourceLinePtr = (char*) source->lockBits() + sLeft * 3 + sTop *
			source->getStride();
		int destHeight = dBottom - dTop + 1;
		int destWidth = dRight - dLeft + 1;
		int sourceHeight = sBottom - sTop + 1;
		int sourceWidth = sRight - sLeft + 1;
		int sourceStride = source->getStride();
		int destStride = getStride();
		int destWrap = destStride - destWidth * 3;

		// Bresenham scaler
		for (int y = dTop; y <= dBottom; y++) {
			char *image_ptr = sourceLinePtr;
			int herror = 0;
			for (int x = dLeft; x <= dRight; x++) {
				destPtr[0] = image_ptr[0];
				destPtr[1] = image_ptr[1];
				destPtr[2] = image_ptr[2];

				herror += sourceWidth;
				while (herror >= destWidth) {
					herror -= destWidth;
 					image_ptr += 3;
				}

				destPtr += 3;
			}

			verror += sourceHeight;
			while (verror > destHeight) {
				verror -= destHeight;
				sourceLinePtr += sourceStride;
			}

			destPtr += destWrap;
		}

		unlockBits();
		source->unlockBits();
	}
}

