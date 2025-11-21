#include <direct.h>
#include <errno.h>
#include <stdio.h>
#include "AxiomShell.h"
#include "autoxor.h"

BOOL BUILTIN_rmdir(SOCKET sock, size_t argc, char **argv)
{
	STACK_RANDOMIZER;
	BOOL res;
	size_t i;
	DWORD ftyp;
	char err_buf[1024];

	memset(err_buf, 0, sizeof(err_buf));
	if (argc == 0)
	{
		JSON_send_packets(JsonCommandOutput, sock, (void *)XorStr("[!] Usage: rmdir [{PATH_1}, {PATH_2}, ...]\n"));
		return (FALSE);
	}

	i = 0;
	res = TRUE;
	while (i < argc)
	{
		ftyp = GetFileAttributesA(argv[i]);
		if (!(ftyp & FILE_ATTRIBUTE_DIRECTORY))
		{
			xsnprintf(err_buf, sizeof(err_buf), "[!] Can not delete %s: not a directory\n", argv[i]);
			JSON_send_packets(JsonCommandOutput, sock, (void*)err_buf);
			i++;
			continue;
		}

		if (_rmdir(argv[i]))
		{
			res = FALSE;
			switch (errno)
			{
				case ENOENT:
					xsnprintf(err_buf, sizeof(err_buf), "[!] Error deleting directory %s: No such file or directory\n", argv[i]);
					JSON_send_packets(JsonCommandOutput, sock, (void*)err_buf);
					break;
				case EACCES:
					xsnprintf(err_buf, sizeof(err_buf), "[!] Error deleting directory %s: Permission denied\n", argv[i]);
					JSON_send_packets(JsonCommandOutput, sock, (void*)err_buf);
					break;
				case ENOTEMPTY:
					xsnprintf(err_buf, sizeof(err_buf), "[!] Error deleting directory %s: Directory not empty or is the current working directory\n", argv[i]);
					JSON_send_packets(JsonCommandOutput, sock, (void*)err_buf);
					break;
				default:
					xsnprintf(err_buf, sizeof(err_buf), "[!] Error deleting directory %s: Unknown error (%d)\n", argv[i], errno);
					JSON_send_packets(JsonCommandOutput, sock, (void*)err_buf);
					break;
			}
		}
		i++;
	}

	return (res);
}

