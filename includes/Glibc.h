#ifndef GLIBC_H
# define GLIBC_H

# include <windows.h>
# include <stdio.h>
# include <stdint.h>
# include <string.h>
# include <stdlib.h>

const int drunk_atoi(const char* argv1);
const char* drunk_strcpy(char* dest, const char* src);
const char* drunk_strcat(char* dest, const char* src);
const char* drunk_strdup(const char* str);
const int drunk_strcmp(const char* s1, const char* s2);
const int drunk_strncmp(const char* s1, const char* s2, unsigned int n);
SIZE_T drunk_strrchr(const char *s, int c);
const char** drunk_strsplit(char const* s, char c);
BOOL drunk_cacheenv(const char** env);
const char* drunk_getenv(const char* key);

#endif

