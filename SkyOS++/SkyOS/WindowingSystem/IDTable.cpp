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
#include "IDTable.h"

#define kTableReallocSize 128

IDTable::IDTable()
	:	fTableSize(0),
		fTable(NULL)
{
}

Atom *IDTable::lookUpAtom(int id)
{
	if (id >= fTableSize || id < 0)
		return NULL;

	return fTable[id];
}

int IDTable::registerAtom(Atom *atom)
{
	int slot = findFreeSlot();
	fTable[slot] = atom;
	atom->setID(slot);
	return slot;
}

void IDTable::freeIdentifier(int id)
{
	assert(id >= 0);
	assert(id < fTableSize);
	fTable[id] = NULL;
}

int IDTable::findFreeSlot()
{
	int i;

	for (i = 0; i < fTableSize; i++) {
		if (fTable[i] == NULL) {
			return i;
		}
	}

	int newTableSize = fTableSize + kTableReallocSize;
	Atom **newTable = new Atom*[newTableSize];
	for (i = 0; i < fTableSize; i++)
		newTable[i] = fTable[i];

	for (i = fTableSize; i < newTableSize; i++)
		newTable[i] = NULL;

	delete [] fTable;
	fTable = newTable;

	int id = fTableSize;
	fTableSize = newTableSize;

	printf("IDTable: grew table to %d entries.  Returning entry %d\n", fTableSize, id);

	return id;
}
