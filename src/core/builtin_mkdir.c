#include <direct.h>
#include <errno.h>
#include <stdio.h>
#include "AxiomShell.h"
#include "autoxor.h"

BOOL BUILTIN_mkdir(SOCKET sock, size_t argc, char **argv)
{
	STACK_RANDOMIZER;
	BOOL res;
	size_t i;
	char err_buf[1024];

	memset(err_buf, 0, sizeof(err_buf));
	if (argc == 0)
	{
		JSON_send_packets(JsonCommandOutput, sock, (void *)XorStr("[!] Usage: mkdir [{PATH_1}, {PATH_2}, ...]\n"));
		return (FALSE);
	}

	i = 0;
	res = TRUE;
	while (i < argc)
	{
		if (_access(argv[i], 0) == 0)
		{
			snprintf(err_buf, sizeof(err_buf), XorStr("[!] Can not create directory %s: A file or directory already exists with this name\n"), argv[i]);
			JSON_send_packets(JsonCommandOutput, sock, (void*)err_buf);
			i++;
			continue;
		}


		if (_mkdir(argv[i]))
		{
			res = FALSE;
			switch (errno)
			{
				case ENOENT:
					snprintf(err_buf, sizeof(err_buf), XorStr("[!] Error creating directory %s: No such file or directory\n"), argv[i]);
					JSON_send_packets(JsonCommandOutput, sock, (void*)err_buf);
					break;
				case EACCES:
					snprintf(err_buf, sizeof(err_buf), XorStr("[!] Error creating directory %s: Permission denied\n"), argv[i]);
					JSON_send_packets(JsonCommandOutput, sock, (void*)err_buf);
					break;
				default:
					snprintf(err_buf, sizeof(err_buf), XorStr("[!] Error creating directory %s: Unknown error (%d)\n"), argv[i], errno);
					JSON_send_packets(JsonCommandOutput, sock, (void*)err_buf);
					break;
			}
		}
		i++;
	}

	return (res);
}

