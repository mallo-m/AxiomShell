#include <errno.h>
#include <direct.h>
#include "AxiomShell.h"
#include "autoxor.h"

BOOL BUILTIN_cd(SOCKET sock, size_t argc, char **argv)
{
	STACK_RANDOMIZER;
	char* path;

	if (argc != 1)
	{
		JSON_send_packets(JsonCommandOutput, sock, (void*)XorStr("[!] Usage: cd [PATH]\n"));
		return (FALSE);
	}

	path = argv[0];
	if (_chdir(path))
	{
		switch (errno)
		{
			case ENOENT:
			    JSON_send_packets(JsonCommandOutput, sock, (void*)XorStr("[!] cd: No such file or directory\n"));
			    break;
			default:
			    JSON_send_packets(JsonCommandOutput, sock, (void*)XorStr("[!] cd: Unknown error\n"));
			    break;
		}
		return (FALSE);
	}
	return (TRUE);
}

