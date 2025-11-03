#include "AxiomShell.h"
#include "ls.h"
#include <stdio.h>

// Display all file-type targets
void BUILTIN_LS_DISPLAY_TARGETS_regular_files(SOCKET sock, PLS_TARGET targets)
{
	DWORD fileAttr;
	CHAR buffer[1024];

	while (targets != NULL)
	{
		if (targets->type == TypeRegularfile)
		{
			fileAttr = GetFileAttributesA(targets->name);
			snprintf(
				buffer, sizeof(buffer),
				"-%c%c%c%c%c%-5s %.2d/%.2d/%.2d %.2d:%.2d	%15lld %s\n",
				(fileAttr & FILE_ATTRIBUTE_ARCHIVE ? 'a' : '-'),
				(fileAttr & FILE_ATTRIBUTE_READONLY ? 'r' : '-'),
				(fileAttr & FILE_ATTRIBUTE_HIDDEN ? 'h' : '-'),
				(fileAttr & FILE_ATTRIBUTE_SYSTEM ? 's' : '-'),
				(fileAttr & FILE_ATTRIBUTE_REPARSE_POINT ? 'l' : '-'),
				"",
				targets->lastWriteTime.wDay, targets->lastWriteTime.wMonth, targets->lastWriteTime.wYear,
				targets->lastWriteTime.wHour, targets->lastWriteTime.wMinute,
				targets->size,
				targets->name
			);
			JSON_send_packets(JsonCommandOutput, sock, (void*)buffer);
		}
		targets = targets->next;
	}
}

// Display all directory-type targets
void BUILTIN_LS_DISPLAY_TARGETS_directories(SOCKET sock, PLS_TARGET targets)
{
	DWORD fileAttr;
	CHAR buffer[1024];

	while (targets != NULL)
	{
		if (targets->type == TypeDirectory)
		{
			if (g_ls_flags & LS_MODE_DIRECTORY)
			{
				fileAttr = GetFileAttributesA(targets->name);
				snprintf(
					buffer, sizeof(buffer),
					"d%c%c%c%c%c%-5s %.2d/%.2d/%.2d %.2d:%.2d	%15s %s\n",
					(fileAttr & FILE_ATTRIBUTE_ARCHIVE ? 'a' : '-'),
					(fileAttr & FILE_ATTRIBUTE_READONLY ? 'r' : '-'),
					(fileAttr & FILE_ATTRIBUTE_HIDDEN ? 'h' : '-'),
					(fileAttr & FILE_ATTRIBUTE_SYSTEM ? 's' : '-'),
					(fileAttr & FILE_ATTRIBUTE_REPARSE_POINT ? 'l' : '-'),
					"",
					targets->lastWriteTime.wDay, targets->lastWriteTime.wMonth, targets->lastWriteTime.wYear,
					targets->lastWriteTime.wHour, targets->lastWriteTime.wMinute,
					"<DIR>",
					targets->name
				);
				JSON_send_packets(JsonCommandOutput, sock, (void*)buffer);
			}
			else {
				BUILTIN_LS_DISPLAY_folder_contents(sock, targets->name);
			}
		}
		targets = targets->next;
	}
}


