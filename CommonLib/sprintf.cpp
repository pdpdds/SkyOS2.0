#include "sprintf.h"
#include <stdarg.h>
#include <va_list.h>
#include <stdio.h>
#include "memory.h"

#include "sprintf.h"
#include "stdint.h"
extern "C" int sprintf(char *s, const char *format, ...)
{
	va_list arg;
	int ret;

	va_start(arg, format);
	ret = vsprintf(s, format, arg);
	va_end(arg);

	return ret;
}


char tbuf[32];
char bchars[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };

void itoa(unsigned i, unsigned base, char* buf) {
	int pos = 0;
	int opos = 0;
	int top = 0;

//������ 16�� �Ѱų� ������ ���� 0�̸� ���� '0'�� buf�� ��´�.
	if (i == 0 || base > 16) {
		buf[0] = '0';
		buf[1] = '\0';
		return;
	}

//������ �°� ���ڸ� ����.
//17�� �Էµǰ� �̸� 16������ ��ȯ�ϸ� 11�̴�.
//�Ʒ� ������ �� ����� ����Ѵ�.
	while (i != 0) {
		tbuf[pos] = bchars[i % base];
		pos++;
		i /= base;
	}
	top = pos--;
	for (opos = 0; opos < top; pos--, opos++) {
		buf[opos] = tbuf[pos];
	}
	buf[opos] = 0;
}

void itoa_s(int i, unsigned base, char* buf) {
	if (base > 16) return;
	if (i < 0) {
		*buf++ = '-';
		i *= -1;
	}
	itoa(i, base, buf);
}

void itoa_s(unsigned int i, unsigned base, char* buf) {
	if (base > 16) return;

	itoa(i, base, buf);
}

#ifndef  SKYOS_WIN32
char* _i64toa(long long value, char *str, int radix)
{
	unsigned long long val;
	int negative;
	char buffer[65];
	char *pos;
	int digit;

	if (value < 0 && radix == 10) {
		negative = 1;
		val = -value;

	}
	else {
		negative = 0;
		val = value;

	} /* if */
	pos = &buffer[64];
	*pos = '\0';
	do {
		digit = val % radix;
		val = val / radix;
		if (digit < 10) {
			*--pos = '0' + digit;

		}
		else {
			*--pos = 'a' + digit - 10;

		} /* if */

	} while (val != 0L);
	if (negative) {
		*--pos = '-';

	} /* if */
	memcpy(str, pos, &buffer[64] - pos + 1);
	return str;
}
#endif // ! WIN32