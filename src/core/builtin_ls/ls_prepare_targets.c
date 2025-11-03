#include "AxiomShell.h"
#include "Glibc.h"
#include "ls.h"
#include <stdio.h>

PLS_TARGET BUILTIN_LS_prepare_targets(SOCKET sock, size_t argc, SIZE_T options_cnt, char **argv)
{
	WIN32_FIND_DATA ffd;
	HANDLE hFind;
	PLS_TARGET targets;
	PLS_TARGET current;
	LARGE_INTEGER fileSize;
	CHAR buffer[1024];

	if (argc == options_cnt) // Meaning the user didn't specify any target, default to '.'
	{
		hFind = FindFirstFileA(".", &ffd);
		current = (PLS_TARGET)malloc(sizeof(LS_TARGET));
		current->type = TypeDirectory;
		current->name = (char *)drunk_strdup(".");
		current->owner = NULL;
		current->size = 0;
		current->next = NULL;
		FileTimeToSystemTime((FILETIME *)&ffd.ftLastWriteTime, (LPSYSTEMTIME)&(current->lastWriteTime));
		FindClose(hFind);
		return (current);
	}

	targets = NULL;
	while (options_cnt < (SIZE_T)argc)
	{
		hFind = FindFirstFileA(argv[options_cnt], &ffd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			snprintf(buffer, sizeof(buffer), "[!] ls: cannot access '%s': No such file or directory\n", argv[options_cnt]);
			JSON_send_packets(JsonCommandOutput, sock, (void*)buffer);
			goto nextelem;
		}

		current = (PLS_TARGET)malloc(sizeof(LS_TARGET));
		current->name = argv[options_cnt];
		current->owner = NULL;
		current->size = 0;
		FileTimeToSystemTime((FILETIME *)&ffd.ftLastWriteTime, (LPSYSTEMTIME)&(current->lastWriteTime));
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			printf("[+] %s is directory\n", argv[options_cnt]);
			current->type = TypeDirectory;
			g_ls_hasdirectories = 1;
		}
		else {
			printf("[+] %s is a regular file\n", argv[options_cnt]);
			current->type = TypeRegularfile;
			fileSize.LowPart = ffd.nFileSizeLow;
			fileSize.HighPart = ffd.nFileSizeHigh;
			current->size = (SIZE_T)fileSize.QuadPart;
			g_ls_hasfiles = 1;
		}
		current->next = targets;
		targets = current;
	nextelem:
		FindClose(hFind);
		options_cnt++;
	}
	return (targets);
}

