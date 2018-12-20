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

// This may not need to be its own class

#ifndef __IDTABLE_H
#define __IDTABLE_H

#include "Bitmap.h"
#include "ServerWindow.h"

class IDTable {
public:
	IDTable();
	void freeIdentifier(int id);
	Atom *lookUpAtom(int id);
	int registerAtom(Atom *);

private:
	int findFreeSlot();

	Atom **fTable;
	int fTableSize;
};

#endif
