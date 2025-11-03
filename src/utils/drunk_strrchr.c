#include "Glibc.h"

SIZE_T drunk_strrchr(const char *s, int c)
{
	SIZE_T i;

	i = strlen(s) + 1;
	while (i > 0)
	{
		i--;
		if (s[i] == c)
			return (i);
	}
	return (-1);
}


