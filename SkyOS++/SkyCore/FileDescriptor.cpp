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

#include "FileDescriptor.h"
#include "VNode.h"

FileDescriptor::FileDescriptor(VNode *node)
	:	Resource(OBJ_FD, ""),
		fNode(node),
		fCurrentPosition(0)
{
}

FileDescriptor::~FileDescriptor()
{
	fNode->ReleaseRef();
}

VNode* FileDescriptor::GetNode() const
{
	return fNode;
}

off_t FileDescriptor::Seek(off_t diff, int whence)
{
	off_t newOffset = -1;
	switch (whence) {
		case SEEK_SET:
			newOffset = diff;
			break;

		case SEEK_CUR:
			newOffset = fCurrentPosition + diff;
			break;

		case SEEK_END:
			newOffset = fNode->GetLength() - newOffset;
			break;
	}
	
	if (newOffset >= 0 && newOffset < fNode->GetLength())
		fCurrentPosition = newOffset;
	
	return fCurrentPosition;
}

int FileDescriptor::ReadDir(char[], size_t)
{
	return E_INVALID_OPERATION;
}

int FileDescriptor::RewindDir()
{
	return E_INVALID_OPERATION;
}

int FileDescriptor::ReadAt(off_t offset, void *buffer, int size)
{
	return fNode->CachedRead(offset, buffer, size);
}

int FileDescriptor::WriteAt(off_t offset, const void *buffer, int size)
{
	return fNode->CachedWrite(offset, buffer, size);
}

int FileDescriptor::Read(void *buf, int size)
{
	int sizeRead = ReadAt(fCurrentPosition, buf, size);
	if (sizeRead > 0)
		fCurrentPosition += sizeRead;
		
	return sizeRead;
}

int FileDescriptor::Write(const void *buf, int size)
{
	int sizeWritten = WriteAt(fCurrentPosition, buf, size);
	if (sizeWritten > 0)
		fCurrentPosition += sizeWritten;
		
	return sizeWritten;
}

int FileDescriptor::Control(int, void*)
{
	return E_INVALID_OPERATION;
}
