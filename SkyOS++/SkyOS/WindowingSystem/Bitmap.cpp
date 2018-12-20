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

#include "Bitmap.h"

Bitmap::Bitmap(ColorSpace colorSpace, int width, int height)
	:	Atom(ATOM_BITMAP),
		MemorySurface(colorSpace, width, height)
{
	fGC = new GraphicsContext(this, 0, 0);
	fGC->setClipRegion(Region(0, 0, width - 1, height - 1));
}

Bitmap::~Bitmap()
{
	delete fGC;
}

GraphicsContext* Bitmap::getGC()
{
	return fGC;
}

