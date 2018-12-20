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

#include "MemorySurface.h"
#include "Rasterizer24bpp.h"

MemorySurface::MemorySurface(ColorSpace colorSpace, int width, int height)
	:	Surface(colorSpace, width, height),
		fBits(new char[width * height * 3])
{
}

MemorySurface::~MemorySurface()
{
	delete [] fBits;
}

void* MemorySurface::lockBits()
{
	return fBits;
}

void MemorySurface::unlockBits()
{
}

int MemorySurface::getStride() const
{
	return getWidth() * (getBitDepth() / 8);
}

Rasterizer *MemorySurface::createRasterizer()
{
	return new Rasterizer24bpp(this);
}

