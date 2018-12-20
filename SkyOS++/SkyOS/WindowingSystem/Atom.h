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

#ifndef __ATOM_H
#define __ATOM_H

enum AtomType {
	ATOM_SERVER_WINDOW,	// XXX change this to just ATOM_WINDOW
	ATOM_BITMAP
};

class Atom {
public:
	Atom(AtomType type)
		:	fType(type)
	{
	}

	virtual ~Atom() { }

	inline void setID(int id) {
		fID = id;
	}

	inline int getID() const {
		return fID;
	}

	inline AtomType getType() const {
		return fType;
	}

private:
	int fID;
	AtomType fType;
};

#endif
