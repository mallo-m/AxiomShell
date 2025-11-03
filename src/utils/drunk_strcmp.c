#include "Glibc.h"

const int drunk_strcmp(const char* s1, const char* s2)
{
	size_t i;

	i = 0;
	while (s1[i] == s2[i] && i < strlen(s1) && i < strlen(s2))
		i++;

	return (s1[i] - s2[i]);
}

