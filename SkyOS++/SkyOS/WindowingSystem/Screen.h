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

#ifndef __SCREEN_H
#define __SCREEN_H

#include "Surface.h"

class Screen : public Surface {
public:
	/// Set the position of the cursor.
	/// @param x Horizontal position of cursor, in pixels
	/// @param y Vertical position of cursor, in pixels
	virtual void setCursorPosition(int x, int y) = 0;

	/// Define the shape of the currently displayed cursor
	/// @param width Width, in pixels, of cursor.  Must be a multiple of 8
	/// @param height Height, in pixels, of cursor.  Can be arbitrary number of pixels
	/// @param colorBits Each bit in this array represents a pixel color.  0 is black, 1 is white
	/// @param maskBits Each bit in this array represents a pixel.  A 1 is a visible pixel,
	///    a zero is an invisible pixel
	/// @todo There should be a way to have a cursor XOR itself onto the frame buffer.  This might
	///    be slow or unavailable on certain hardware architectures.
	virtual void setCursorShape(int width, int height, const char *colorBits, const char *maskBits) = 0;

protected:
	Screen(ColorSpace colorSpace, int width, int height)
		:	Surface(colorSpace, width, height)
	{
	}
};

#endif

