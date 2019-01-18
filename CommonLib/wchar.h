#pragma once
#include "windef.h"
#include "stdint.h"

#if !defined(__cplusplus)
typedef short wchar_t;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct
	{
		wchar_t __cp;
		uint16_t __class;
		wchar_t __uc;
		wchar_t __lc;
	} __wchar_info_t;

	wchar_t *wcscat(wchar_t*__restrict dest, const wchar_t*__restrict src);
	wchar_t* wcschr(const wchar_t* str, wchar_t c);
	int wcscmp(const wchar_t *str1, const wchar_t *str2);
	wchar_t *wcscpy(wchar_t *__restrict strDestination, const wchar_t *__restrict strSource);
	size_t wcslen(const wchar_t *str);
	int wcsncmp(const wchar_t *s1, const wchar_t *s2, size_t n);
	wchar_t* wcsncpy(wchar_t* __restrict Dst, const wchar_t* __restrict Src, unsigned int Count);
	wchar_t* wcspbrk(const wchar_t *s1, const wchar_t *s2);
	size_t wcstombs(char *mbstr, const wchar_t *wcstr, size_t count);	
	int _wcsnicmp(const wchar_t *s1, const wchar_t *s2, size_t n);		
	wchar_t *wmemcpy(wchar_t *__restrict d, const wchar_t *__restrict s, size_t n);
	int _wcwidth(wchar_t ucs);
	int _wcswidth(const wchar_t *pwcs, size_t n);
	int _wcswidth_cjk(const wchar_t *pwcs, size_t n);
	size_t wcslcpy(wchar_t *dst, const wchar_t *src, size_t siz);

#ifdef __cplusplus
}
#endif