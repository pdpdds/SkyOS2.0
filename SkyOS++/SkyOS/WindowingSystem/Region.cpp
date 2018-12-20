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
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "util.h"
#include "Region.h"
#include "debug.h"
#include "SystemAPI.h"

const int kReallocSize = 32;

const Region Region::kNull;
Region* kInfinite;

Region::Region()
	:	fNumRects(0),
		fRects(NULL)
{
}

Region::Region(int left, int top, int right, int bottom)
	:	fNumRects(0),
		fRects(NULL)
{
	addRect(Rect(left, top, right, bottom));
}

Region::Region(const Region &copyRegion)
	:	fNumRects(0),
		fRects(NULL)
{
	operator=(copyRegion);
}

Region::~Region()
{
	kfree(fRects);
}

Region& Region::operator=(const Region &copyRegion)
{
	if (&copyRegion == this)
		return *this;

	// Clear out existing rects
	kfree(fRects);
	fRects = 0;
	fNumRects = 0;

	// Copy in new ones
	for (int i = 0; i < copyRegion.fNumRects; i++)
		addRect(copyRegion.fRects[i]);

	return *this;
}

bool Region::isEmpty() const
{
	return fNumRects == 0;
}

Rect Region::getBounds() const
{
	Rect bounds(0x7fffffff, 0x7fffffff, 0x80000000, 0x80000000);

	for (int i = 0; i < fNumRects; i++) {
		bounds.left = min(bounds.left, fRects[i].left);
		bounds.right = max(bounds.right, fRects[i].right);
		bounds.top = min(bounds.top, fRects[i].top);
		bounds.bottom = max(bounds.bottom, fRects[i].bottom);
	}

	return bounds;
}

bool Region::contains(int x, int y) const
{
	for (int i = 0; i < fNumRects; i++) {
		if (fRects[i].contains(x, y))
			return true;
	}

	return false;
}

Rect Region::getBoundingRect(int x, int y) const
{
	for (int i = 0; i < fNumRects; i++) {
		if (fRects[i].contains(x, y))
			return fRects[i];
	}

	return Rect(0, 0, 0, 0);
}

Region Region::operator&(const Region &region) const
{
	Region b(*this);
	b.exclude(~region);

	return b;
}

Region Region::operator|(const Region &region) const
{
	Region a(*this);
	Region b(region);
	b.exclude(a);
	a.include(b);

	return a;
}

Region Region::operator-(const Region &region) const
{
	Region r(*this);
	r.exclude(region);

	return r;
}

Region Region::operator^(const Region &region) const
{
	Region a(*this);
	Region b(region);

	b.exclude(a);
	a.exclude(b);

	return a | b;
}

Region Region::operator~() const
{
	return *kInfinite - *this;
}

Region& Region::operator&=(const Region &region)
{
	exclude(~region);
	return *this;
}

Region& Region::operator|=(const Region &region)
{
	Region b(region);
	b.exclude(*this);
	include(b);

	return *this;
}

Region& Region::operator-=(const Region &region)
{
	exclude(region);

	return *this;
}

Region& Region::operator^=(const Region &region)
{
	*this = *this ^ region;
	return *this;
}

Region& Region::translateBy(int xv, int yv)
{
	for (int i = 0; i < fNumRects; i++)
		fRects[i].offsetBy(xv, yv);

	return *this;
}

void Region::clip(int left, int top, int right, int bottom, Clipper &clipper) const
{
	Rect boundingRect(left, top, right, bottom);

	// Make the boundingRect be right side out
	if (boundingRect.right < boundingRect.left) {
		int temp = boundingRect.right;
		boundingRect.right = boundingRect.left;
		boundingRect.left = temp;
	}

	if (boundingRect.bottom < boundingRect.top) {
		int temp = boundingRect.bottom;
		boundingRect.bottom = boundingRect.top;
		boundingRect.top = temp;
	}

	for (int i = 0; i < fNumRects; i++) {
		if (fRects[i].intersects(boundingRect)) {
			clipper(fRects[i].left, fRects[i].top, fRects[i].right,
				fRects[i].bottom);
		}
	}
}

void Region::optimize()
{
	int count = fNumRects;

	for (int i = 0; i < count; i++) {
		for (int j = i; j < count; ) {
			if (fRects[i].top == fRects[j].top
				&& fRects[i].bottom == fRects[j].bottom
				&& fRects[i].right == fRects[j].left) {
				// Merge these two rects, eliminate the second one
				fRects[i].right = fRects[j].right;
				for (int k = j; k < count; k++)
					fRects[k] = fRects[k + 1];

				count--;
			} else
				j++;
		}
	}

	fNumRects = count;
}

void Region::print() const
{
	printf("Region Dump -------------------\r\n");
	for (int i = 0; i < fNumRects; i++)
		fRects[i].print();

	printf("\r\n");
}

void Region::include(const Region &region)
{
	for (int i = 0; i < region.fNumRects; i++)
		addRect(region.fRects[i]);
}

void Region::exclude(const Region &region)
{
	for (int i = 0; i < region.fNumRects; i++)
		exclude(region.fRects[i]);
}

void Region::exclude(const Rect &exrect)
{
	int i = 0;
	int checkRects = fNumRects;
	while (i < checkRects) {
		Rect clipRect = fRects[i];

		if (!exrect.intersects(clipRect)) {
			i++;
			continue;
		}

		// The clip rect could be divided into as many as 8 pieces.
		// Check to see if each one exists and add if so.
		// +-----------------+-----------------+-----------------+
		// |                 |                 |                 |
		// |                 |                 |                 |
		// |       Q1        |       Q2        |       Q3        |
		// |                 |                 |                 |
		// |                 |                 |                 |
		// +-----------------+-----------------+-----------------+
		// |                 |/ / / / / / / / /|                 |
		// |                 |/ / / / / / / / /|                 |
		// |       Q4        |/ /  exrect / / /|       Q5        |
		// |                 |/ / / / / / / / /|                 |
		// |                 |/ / / / / / / / /|                 |
		// +-----------------+-----------------+-----------------+
		// |                 |                 |                 |
		// |                 |                 |                 |
		// |       Q6        |       Q7        |       Q8        |
		// |                 |                 |                 |
		// |                 |                 |                 |
		// +-----------------+-----------------+-----------------+

		Rect q1(clipRect.left, clipRect.top, exrect.left - 1, exrect.top - 1);
		if (clipRect.intersects(q1)) {
			q1.intersect(clipRect);
			addRect(q1);
		}

		Rect q2(exrect.left - 1, clipRect.top, exrect.right + 1, exrect.top - 1);
		if (clipRect.intersects(q2)) {
			q2.intersect(clipRect);
			addRect(q2);
		}

		Rect q3(exrect.right + 1, clipRect.top, clipRect.right, exrect.top - 1);
		if (clipRect.intersects(q3)) {
			q3.intersect(clipRect);
			addRect(q3);
		}

		Rect q4(clipRect.left, exrect.top - 1, exrect.left - 1, exrect.bottom + 1);
		if (clipRect.intersects(q4)) {
			q4.intersect(clipRect);
			addRect(q4);
		}

		Rect q5(exrect.right + 1, exrect.top - 1, clipRect.right, exrect.bottom + 1);
		if (clipRect.intersects(q5)) {
			q5.intersect(clipRect);
			addRect(q5);
		}

		Rect q6(clipRect.left, exrect.bottom + 1, exrect.left - 1, clipRect.bottom);
		if (clipRect.intersects(q6)) {
			q6.intersect(clipRect);
			addRect(q6);
		}

		Rect q7(exrect.left - 1, exrect.bottom + 1, exrect.right + 1, clipRect.bottom);
		if (clipRect.intersects(q7)) {
			q7.intersect(clipRect);
			addRect(q7);
		}

		Rect q8(exrect.right + 1, exrect.bottom + 1, clipRect.right, clipRect.bottom);
		if (clipRect.intersects(q8)) {
			q8.intersect(clipRect);
			addRect(q8);
		}

		// This rect has been split, remove it.  Note we don't
		// change the index
		removeRect(i);
		checkRects--;
	}
}

void Region::addRect(const Rect &rect)
{
	if (fNumRects == 0)
		fRects = (Rect*) malloc(kReallocSize * sizeof(Rect));
	else if (fNumRects % kReallocSize == 0)
		fRects = (Rect*) krealloc(fRects, (fNumRects + kReallocSize) *
			sizeof(Rect));

	fRects[fNumRects++] = rect;
}

void Region::removeRect(int index)
{
	fNumRects--;
	for (int i = index; i < fNumRects; i++)
		fRects[i] = fRects[i + 1];
}

