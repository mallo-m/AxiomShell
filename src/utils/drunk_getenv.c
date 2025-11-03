#include "Glibc.h"

const char** _env;

BOOL drunk_cacheenv(const char** env)
{
	_env = env;
	return (TRUE);
}

const char* drunk_getenv(const char* key)
{
	size_t counter;

	counter = 0;
	while (_env[counter] != NULL)
	{
		//printf("%s\n", _env[counter]);
		if (drunk_strncmp(key, _env[counter], strlen(key)) == 0 && _env[counter][strlen(key)] == '=')
		{
			return (drunk_strdup(_env[counter] + strlen(key) + 1));
		}
		counter++;
	}
	return (NULL);
}

