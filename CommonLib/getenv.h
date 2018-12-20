#pragma once
#ifdef __cplusplus
extern "C" {
#endif
	char *getenv(const char *name);
	int putenv(char *string);
#ifdef __cplusplus
}
#endif