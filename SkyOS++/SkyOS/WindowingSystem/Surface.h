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

#ifndef __SURFACE_H
#define __SURFACE_H

class Rasterizer;

/// Surface is a backing store for raster data.  Any data in
/// rasterized form is stored in some type of surface.
/// The underlying memory can be system memory, or memory on
/// a video card.
/// @note Add method to set a palette for this surface
class Surface {
public:
	enum ColorSpace {
		kCMAP8,
		kRGB565,
		kRGB24,
		kRGBA32
	};

	Surface(ColorSpace colorSpace, int width, int height);
	virtual ~Surface();

	/// Lock the data of the surface in memory and return a pointer to
	/// the data
	/// @returns Pointer to the upper left pixel of the surface
	/// @note Perhaps should not be able to lock a surface that is the source
	///   or destination of a blit
	/// @note This could potentially take a rectangle of the region to lock
	virtual void* lockBits() = 0;

	/// Unlock data of surface so it can be moved around in memory
	virtual void unlockBits() = 0;

	/// Get the stride of the memory returned by lockBits.
	/// @note This function only returns a valid value if lockBits has been called.
	/// @returns Number of bytes between a pixel and the pixel on the same
	///  same column but one row lower
	virtual int getStride() const = 0;

	/// @returns Color space format
	ColorSpace getColorSpace() const;

	/// @returns Width of the surface in pixels
	int getWidth() const;

	/// @returns Height of the surface in pixels
	int getHeight() const;

	/// @returns Number of bits in a pixel
	int getBitDepth() const;

	/// Copy data from one surface to this one
	/// @param source Surface to copy from
	/// @param sLeft Left side of source rectangle
	/// @param sTop Top of source rectangle
	/// @param sRight Right side of source rectangle
	/// @param sBottom Bottom of source rectangle
	/// @param dLeft Left side of destination rectangle
	/// @param dTop Top of destination rectangle
	/// @param dRight Right side of destination rectangle
	/// @param dBottom Bottom of destination rectangle.
	virtual void blit(Surface *source, int sLeft, int sTop, int sRight, int sBottom,
		int dLeft, int dTop, int dRight, int dBottom);

	/// Create a rasterizer compatible with this surface
    /// @returns
    ///  - Pointer to a rasterizer object
    ///  - NULL if an error occurs.
	virtual Rasterizer *createRasterizer() = 0;

private:
	ColorSpace fColorSpace;
	int fWidth;
	int fHeight;
};

#endif
