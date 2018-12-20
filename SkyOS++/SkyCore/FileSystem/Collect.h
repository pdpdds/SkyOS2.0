#pragma once

#include "windef.h"

//#include <Heap.h>
//#include <stdio.h>
#ifndef __cplusplus
#error You must use this header file in C++ programs
#endif
template <class PtrType>
struct CollectionData
{
	PtrType ptr;
	char Key[15];
	CollectionData * Next;
};

template <class PtrType>
class Collection
{
private:
	CollectionData <PtrType>* _Item;
	UINT16 ItemCount;
public:
	Collection()
	{
		Initialize();
	}
	~Collection()
	{
		Clear();
	}
	void Initialize()
	{
		_Item = 0;
		ItemCount = 0;
	}
	UINT16 Count();
	BOOLEAN Add(PtrType ptr, int Index, char * _key = "");
	BOOLEAN Add(PtrType ptr);
	BOOLEAN Add(PtrType ptr, char * Key);
	BOOLEAN IsKeyAlreadyExist(char * Key);
	BOOLEAN Remove(int Index);
	void Clear();
	PtrType Item(int Index);
	PtrType operator()(int Index);
	PtrType Item(char * Key);
	PtrType operator()(char * Key);
};
template<class PtrType> UINT16 Collection<PtrType>::Count()
{
	return ItemCount;
}

template<class PtrType> BOOLEAN Collection<PtrType>::Add(PtrType ptr, int Index, char * PKey)
{
	CollectionData <PtrType> *ptrPrev, *ptrNext = 0, *ptrNew, *ptrTemp;
	char Key[15], i = 0;
	UINT16 Pos = 0;
	while (PKey[i] && i < 14)
	{
		Key[i] = PKey[i];
		i++;
	}

	Key[i] = 0;
	ptrTemp = ptrPrev = this->_Item;		//this loop will find the position where we have to put the new node
	while (Pos < Index && ptrTemp)
	{
		ptrTemp = ptrTemp->Next;
		if (ptrTemp)
			ptrPrev = ptrPrev->Next;
		Pos++;
	}

	ptrNext = ptrPrev ? ptrPrev->Next : NULL; 	// you must insert before this node so keep this node's address
	//--------------Allocate New node and Initializing its values

	ptrNew = (CollectionData <PtrType> *) new CollectionData <PtrType>();
	if (ptrNew == NULL)
		return NULL;
	ptrNew->ptr = ptr;
	strcpy(ptrNew->Key, Key);
	ptrNew->Next = ptrNext;	  // placeing any other old nodes after that one
	if (ptrPrev)
		ptrPrev->Next = ptrNew;     // putting the new node in the correct place
	else
		this->_Item = ptrNew;
	ItemCount++;		 // Incrementing the total
	return TRUE;
}

template <class PtrType> BOOLEAN Collection<PtrType>::Add(PtrType ptr)
{
	return Add(ptr, ItemCount);
}
template <class PtrType> BOOLEAN Collection<PtrType>::IsKeyAlreadyExist(char * Key)
{
	CollectionData <PtrType>* ptrCur = this->_Item;
	while (ptrCur)
	{
		if (strcmp(Key, ptrCur->Key) == 0)
			return TRUE;
		ptrCur = ptrCur->Next;
	}
	return FALSE;
}
template <class PtrType> BOOLEAN Collection<PtrType>::Add(PtrType ptr, char * Key)
{
	CollectionData <PtrType>* ptrCur = this->_Item;
	while (ptrCur)
	{
		if (strcmp(Key, ptrCur->Key) == 0)
			return FALSE;
		ptrCur = ptrCur->Next;
	}

	return Add(ptr, ItemCount, Key);
}
/* ----------------Donot work correctly on some occasions-------------*/
template<class PtrType> BOOLEAN Collection<PtrType>::Remove(int Index)
{
	CollectionData <PtrType>**ptrCur = &this->_Item, **ptrPrev = 0;
	UINT16 i = 0;
	while (*ptrCur && i <= Index)
	{
		if (i == Index)
		{
			if (ptrPrev == 0)
				this->_Item = (*ptrCur)->Next;
			else
				(*ptrPrev)->Next = (*ptrCur)->Next;
			delete *ptrCur;
			ItemCount--;
			return TRUE;
		}
		ptrPrev = &*ptrCur;
		ptrCur = &(*ptrCur)->Next;
		i++;
	}
	return FALSE;
}

template<class PtrType> void Collection<PtrType>::Clear()
{
	CollectionData  <PtrType> * ptrCur = _Item;
	if (ptrCur != NULL)
		ptrCur = this->_Item->Next;
	else
		ptrCur = NULL;
	while (ptrCur && this->_Item)
	{
		CollectionData  <PtrType>* ptrNext = ptrCur->Next;   //Storing the Next Value
   
		delete ptrCur;			      //Deleting pointer
		ptrCur = ptrNext;			      //Next
	}
	this->_Item = 0;
	ItemCount = 0;
}
template<class PtrType> PtrType Collection<PtrType>::Item(int Index)
{
	CollectionData <PtrType>* ptrCur = this->_Item;
	UINT16 i = 0;
	while (ptrCur)
	{
		if (i == Index)
			return ptrCur->ptr;
		ptrCur = ptrCur->Next;
		i++;
	}
	return 0;
}
template<class PtrType> PtrType Collection<PtrType>::Item(char * Key)
{
	CollectionData <PtrType>* ptrCur = this->_Item;
	while (ptrCur)
	{
		if (strcmp(Key, ptrCur->Key) == 0)
			return ptrCur->ptr;
		ptrCur = ptrCur->Next;
	}
	return 0;
}

template<class PtrType> PtrType Collection<PtrType>::operator()(int Index)
{
	return Item(Index);
}
template<class PtrType> PtrType Collection<PtrType>::operator()(char *Key)
{
	return Item(Key);
}
