// 
// Copyright 1998-2012 Jeff Bush
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

/// @file Resource.h
#ifndef _RESOURCE_H
#define _RESOURCE_H

#include "types.h"
#include "Synchronization.h"

/// User-visible operating system primitives derive from Resource.  It has shared ownership semantics
/// and is generally stuck into the handle table.
class Resource : public Synchronization {
public:

	/// @note the ResourceType enumeration is a bit of a hack.
	Resource(ResourceType, const char name[]);
	virtual ~Resource();

	/// Return the type of this resource
	/// @bug This should return the ResourceType enumeration, not an integer
	inline int GetType() const;

	/// Return the name of this resource.
	/// @note This object retains ownership of the returned string.  Objects should
	/// not hold onto the string returned unless they have a reference to this object,
	/// otherwise it may go away
	const char* GetName() const;

	/// Set the name of this object.  The name is restricted to be OS_NAME_LENGTH or
	/// less.  If the passed name is greater in length than that, it will be truncated.
	void SetName(const char[]);

	/// Increment the ref count of this object
	void AcquireRef();

	/// Decrement the reference count of this object.  If the reference count
	/// reaches zero, this object will be deleted
	/// @note This function may block, so don't use it from interrupt context.
	void ReleaseRef();

	/// Print basic information about this resource
	/// @note Should this be virtual to each resource can print its own information?
	void Print() const;

private:
	ResourceType fType;
	volatile int fRefCount;
	char fName[OS_NAME_LENGTH];
};

inline int Resource::GetType() const
{
	return fType;
}

#endif
