#pragma once
#include <size_t.h>
//표준 C 런타임 라이브러리
#include "va_list.h"

#ifdef  __cplusplus
extern "C" {
#endif

char *strcpy(char *s1, const char *s2);
size_t strlen ( const char* str );
int strcmp (const char* str1, const char* str2);
int strncmp(const char *s1, const char *s2, size_t n);
int strncasecmp(const char *s1, const char *s2, size_t n);
char* strstr(const char *in, const char *str);
char *strupr(char *str);
char *strcat(char *dest, const char *src);
char * strncat(char * destination, const char * source, size_t num);
char* strchr (const char * str, int character );
int stricmp(const char *s1, const char *s2);

char *strpbrk(const char *strSrc, const char *str);
char * strtok(char *s1, const char *delimit);
int strspn(const char *strSrc, const char *str);
char *strncpy(char *string1, const char *string2, size_t count);
int strnicmp(const char * String1, const char * String2, unsigned int Len);
char *strnchr(const char *str, char c, size_t count);
const char *strrchr(const char * String, char const Character);
size_t strxfrm(char *dst, char *src, size_t n);
int strcoll(const char *s1, const char *s2);
double strtod(const char *string, char **endPtr);
size_t strcspn(const char* s1, const char *s2);
unsigned long strtoul(const char *nptr, char **endptr, int base);

void ftoa_fixed(char *buffer, double value);
void ftoa_sci(char *buffer, double value);
char * strichr(char *p, int c);
void vsnprintf(char *out, int size, const char *format, va_list args);
void snprintf(char *out, int size, const char *fmt, ...);
char*	strndup(const char *s, size_t n);
char*	strdup(const char *s);

char* strtok_r(char *s, const char *delim, char **save_ptr);

int stricmp(const char *s1, const char *s2);
int strnicmp(const char *s1, const char *s2, size_t n);

#ifdef  __cplusplus
}
#endif