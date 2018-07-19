#pragma once
#include "windef.h"
#include "stddef.h"

#define SEEK_SET        0               /* seek to an absolute position */
#define SEEK_CUR        1               /* seek relative to current position */
#define SEEK_END        2               /* seek relative to end of file */

//���� �÷���
#define FS_FILE       0
#define FS_DIRECTORY  1
#define FS_INVALID    2

typedef struct _FILE {

	char     _name[32]; //���� �̸�
	DWORD	 _flags; //�÷���
	DWORD    _fileLength; //���� ����
	DWORD    _id; //���̵�, �Ǵ� �ڵ�
	DWORD    _eof; //������ ���� �����ߴ°�
	DWORD    _position; // ���� ��ġ
	DWORD    _currentCluster; //���� Ŭ������ ��ġ
	DWORD    _deviceID; //����̽� ���̵�

}FILE, *PFILE;

#ifdef  __cplusplus
extern "C" {
#endif

	extern size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
	extern FILE *fopen(const char *filename, const char *mode);
	extern size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
	extern int fclose(FILE *stream);
	extern int feof(FILE *stream);
	extern int ferror(FILE *stream);
	extern int fflush(FILE *stream);
	extern FILE *freopen(const char *filename, const char *mode, FILE *stream);
	extern int fseek(FILE *stream, long int offset, int whence);
	extern long int ftell(FILE *stream);
	extern int fgetc(FILE * stream);
	extern char* fgets(char *dst, int max, FILE *fp);
	int fscanf(FILE *fp, const char *fmt, ...);
	extern char*	strerr(int errnum);
	//extern FILE*	stderr;
	extern int fprintf(FILE * stream, const char * format, ...);
	char *	strerror(int errnum);

#ifdef  __cplusplus
}
#endif


