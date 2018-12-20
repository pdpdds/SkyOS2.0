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

/// @file Region.h
///

#ifndef __REGION_H
#define __REGION_H

#include "Rect.h"
#include "Clipper.h"

/// A Region, mathematically, is a set of points on a 2 dimensional plane.
///
/// @note There is no function to randomly access rectangles in the region.  This would assume
///    that the region is comprised of rectangles, which is doesn't necessarily need to be.  Instead, the Clipper
///    interface is used to decompose the region into rectangles and iterate over them.  As much as possible, this
///    class attempts to hide the actual storage of the region data.  This allows more easily changing
///    the internal representation to be more efficient (for example, sorting rectangles or using a different
///    primitive altogether)
/// @note Currently regions are inclusive-inclusive
class Region {
public:
	/// A region containing no points
	static const Region kNull;

	/// A region containing every point possible.
	

	/// Creates an empty region (contains no points)
	Region();

	/// Copy constructor
	Region(const Region&);

	/// Create a region that contains all points within a specified rectangle.
	/// @param left Left side of rectangle (inclusive)
	/// @param top Top of rectangle (inclusive)
	/// @param right Right side of rectangle (inclusive)
	/// @param bottom Bottom of rectangle (inclusive)
	Region(int left, int top, int right, int bottom);
	~Region();
	Region& operator=(const Region&);

	bool isEmpty() const;

	/// Returns the smallest rectangle that will contain all of the points
	/// in this region.
	Rect getBounds() const;

	/// Test if a point is a member of this region
	/// @param x Horizontal coordinate of point to test
	/// @param y Vertical coordinate of point to test
	/// @returns
	///    - true if this point is a member of this region
	///    - false if this point is not a member of this region
	bool contains(int x, int y) const;

	/// Returns a rectangle, of which the point x,y is a member and
	/// of which all points are members of this region.  This rectangle
	/// will either be a subset of the region or will contain all of the points
	/// of the region (if the region is a rectangle)
	/// @param x Horizontal coordinate of point
	/// @param y Vertical coordinate of point
	/// @returns Rect structure that contains said points
	Rect getBoundingRect(int x, int y) const;

	/// Intersection of this region with another
	/// @returns Region containing all points that exist both in this region and the passed region
	Region operator&(const Region &region) const;

	/// Union of this region with another
	/// @returns Region containing all points that exist either in this region or the passed region
	Region operator|(const Region &region) const;

	/// Difference of this region with another
	/// @returns Region containing all points that exist in this region and do not exist
	///  in the passed region
	Region operator-(const Region &region) const;

	/// Exclusive or of this region with another
	/// @returns Region containing all points that exist in either this region or the passed
	///  region, but not both.
	Region operator^(const Region &region) const;

	/// Complement of this region.  This is basically an infinite region minus this region
	/// @returns Region containing all points that do not exist in this region
	Region operator~() const;

	Region& operator&=(const Region &region);
	Region& operator|=(const Region &region);
	Region& operator-=(const Region &region);
	Region& operator^=(const Region &region);
	Region& translateBy(int x, int y);

	/// Use region to clip some primitive.  This function decomposes the
	/// region into rectangles and repeatedly calls the clippper functor on
	/// each one.  Don't assume that the rectangles will be in any specific
	/// order.
	/// @param left Leftmost boundary of primitive.  The clipper will
	///    not be called with any rectangles that are completely to the left
	///    of this coordinate.
	/// @param top Top boundary of primitive
	/// @param right Right boundary of primitive
	/// @param bottom Bottom boundary of primitive
	/// @param clipper function that will be called for each rectangle in the region
	///
	/// @note Left must be less than right and top must be less than or equal to bottom
	//      All coordinates are inclusive
	///
	void clip(int left, int top, int right, int bottom, Clipper &clipper) const;

	/// More of a debugging function really.  This attempts to merge
	/// adjacent rectangles to reduce the total number of rectangles in the
	/// region and make drawing faster.  This is a pretty slow operation
	void optimize();

	/// Print internal data of this region to standard out.
	void print() const;

private:
	/// Add all points in the passed region to this region.  Note, this assumes that all
	/// points in the passed region are not already in this region.  If you pass a region
	/// for which this is not true, you will create an invalid region that contains
	/// overlapping rectangles
	void include(const Region&);

	/// Remove all points in the passed region from this region.  It is okay if the
	/// passed rectangle contains points that are not in this region.
	void exclude(const Region&);

	/// Remove all points in the passed rectangle from this region.  It is okay if the
	/// passed rectangle contains points that are not in this region.
	void exclude(const Rect&);

	/// Remove all points in this region
	void clear();

	/// Add all points from a rectangle to this region.  Note that this assumes that
	/// all points in the given rectangle do not exist in the region.  If there are points
	/// that are already in the region, you will create an invalid region that contains overlapping
	/// points.
	void addRect(const Rect&);

	/// Remove the rectangle at the given index from the region, moving all subsequent rectangles
	/// to fill the space.
	void removeRect(int index);

	/// Array of rectangles that comprise this region.
	Rect *fRects;

	/// Number of rectangles in fRects array
	int fNumRects;
};

#endif
