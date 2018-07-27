#pragma once

extern void *operator new(size_t size);
extern void *operator new[](size_t size);
extern void *operator new(size_t, void *p);
extern void *operator new[](size_t, void *p);

void operator delete(void *p);
void operator delete(void *p, size_t size);
void operator delete[](void *p);
void operator delete[](void *p, size_t size);

int __cdecl _purecall();