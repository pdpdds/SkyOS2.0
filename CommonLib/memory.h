#pragma once

#ifdef  __cplusplus
extern "C" {
#endif
void* memcpy(void *dest, const void *src, size_t count);
void* memset(void *dest, char val, size_t count);
unsigned short* memsetw(unsigned short *dest, unsigned short val, size_t count);
int memcmp(const void *s1, const void *s2, size_t n);
void* memmove(void *dest, void *src, size_t n);
void* memchr(const void *s, int c, size_t n);
void* malloc(size_t size);
void free(void *p);
#ifdef  __cplusplus
}
#endif
