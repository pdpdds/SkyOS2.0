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

#ifndef RECT_H
#define RECT_H

#include "stdio.h"
#include "debug.h"
#include "util.h"

/// @note I'd like to get rid of Rect
class Rect {
public:

	int left;
	int top;
	int right;
	int bottom;

	inline Rect();
	inline Rect(int left, int top, int right, int bottom);
	inline Rect& insetBy(int, int);
	inline Rect& offsetBy(int, int);
	inline Rect& offsetTo(int, int);
	inline bool contains(int, int) const;
 	inline bool contains(const Rect&) const;
	inline bool intersects(const Rect&) const;
	inline Rect& intersect(const Rect&);
	inline bool valid() const;
	inline int width() const;
	inline int height() const;
	inline void print() const;
};

inline Rect::Rect()
	:	left(0),
		right(0),
		top(0),
		bottom(0)
{
}

inline Rect::Rect(int l, int t, int r, int b)
	:	left(l),
		top(t),
		right(r),
		bottom(b)
{
}

inline Rect& Rect::insetBy(int h, int v)
{
	left += h;
	right -= h;
	top += v;
	bottom -= v;
	return *this;
}

inline Rect& Rect::offsetBy(int h, int v)
{
	left += h;
	right += h;
	top += v;
	bottom += v;
	return *this;
}


inline Rect& Rect::offsetTo(int h, int v)
{
	right += (h - left);
	bottom += (v - top);
	left = h;
	top = v;
	return *this;
}

inline bool Rect::intersects(const Rect &rect) const
{
	return (left <= rect.right && right >= rect.left
		&& bottom >= rect.top && top <= rect.bottom);
}

inline bool Rect::valid() const
{
	return right >= left && bottom >= top;
}

inline void Rect::print() const
{
	printf("Rect (%d, %d, %d, %d)\r\n", left, top, right, bottom);
}


inline int Rect::width() const
{
	return right - left + 1;	// XXX shouldn't this be + 1?
}

inline int Rect::height() const
{
	return bottom - top + 1;	// XXX shouldn't this be + 1?
}

inline bool Rect::contains(int x, int y) const
{
	return (x >= left && x <= right && y >= top && y <= bottom);
}

inline bool Rect::contains(const Rect &rect) const
{
	return rect.left >= left && rect.right <= right
		&& rect.top >= top && rect.bottom <= bottom;
}

inline Rect& Rect::intersect(const Rect &rect)
{
	left = max(left, rect.left);
	right = min(right, rect.right);
	top = max(top, rect.top);
	bottom = min(bottom, rect.bottom);
	return *this;
}

#endif
