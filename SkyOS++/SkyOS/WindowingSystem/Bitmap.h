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

#ifndef __BITMAP_H
#define __BITMAP_H

#include "Atom.h"
#include "MemorySurface.h"
#include "GraphicsContext.h"

///
/// A bitmap:
///  - is an atom (and thus is accessible by clients)
///  - Has an associated graphics context.
///
class Bitmap : public MemorySurface, public Atom {
public:
	Bitmap(ColorSpace colorSpace, int width, int height);
	~Bitmap();
	GraphicsContext *getGC();

private:
	GraphicsContext *fGC;
};

#endif
