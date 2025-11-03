#include "Glibc.h"
#include "Utils.h"
#include "Typedefs.h"
#include "stringapiset.h"
#include <stdio.h>
#include <minwinbase.h>
#include <string.h>
#include <wchar.h>
#include <direct.h>

#define ERROR_BINARY "[!] Binary or command not found\n"
#define ERROR_PIPE "[!] Failed to create PIPE, please try again\n"
#define ERROR_GENERIC "[!] Unexpected error\n"

typedef NTSTATUS(NTAPI* _C_NtCreateUserProcess)(
    _Out_ PHANDLE ProcessHandle,
    _Out_ PHANDLE ThreadHandle,
    _In_ ACCESS_MASK ProcessDesiredAccess,
    _In_ ACCESS_MASK ThreadDesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ProcessObjectAttributes,
    _In_opt_ POBJECT_ATTRIBUTES ThreadObjectAttributes,
    _In_ ULONG ProcessFlags,
    _In_ ULONG ThreadFlags,
    _In_ PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
    _Inout_ PPS_CREATE_INFO CreateInfo,
    _In_ PPS_ATTRIBUTE_LIST AttributeList
    );

typedef NTSTATUS(NTAPI* _C_RtlCreateProcessParametersEx)(
    OUT PRTL_USER_PROCESS_PARAMETERS* pProcessParameters,
    IN PUNICODE_STRING ImagePathName,
    IN PUNICODE_STRING DllPath,
    IN PUNICODE_STRING CurrentDirectory,
    IN PUNICODE_STRING CommandLine,
    IN PVOID Environment,
    IN PUNICODE_STRING WindowTitle,
    IN PUNICODE_STRING DesktopInfo,
    IN PUNICODE_STRING ShellInfo,
    IN PUNICODE_STRING RuntimeData,
    IN ULONG Flags
    );

typedef VOID(NTAPI* _C_RtlInitUnicodeString)(
    OUT PUNICODE_STRING DestinationString,
    IN PCWSTR SourceString
    );

typedef PVOID(NTAPI* _C_RtlAllocateHeap)(
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN SIZE_T Size
    );

typedef int(NTAPI* _C_RtlFreeHeap)(
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID HeapBase
    );

static void InitializeObjectAttributes(
    POBJECT_ATTRIBUTES p,
    PUNICODE_STRING n,
    ULONG a,
    HANDLE r,
    PVOID s
) {
    p->Length = sizeof(OBJECT_ATTRIBUTES);
    p->RootDirectory = r;
    p->Attributes = a;
    p->ObjectName = n;
    p->SecurityDescriptor = s;
    p->SecurityQualityOfService = (void*)0;
}

static const char* lookupBinaryInDir(const char* binary, const char* dir)
{
	char searchBuffer[260];
	char resultBuffer[1024];
	HANDLE hFind;
	WIN32_FIND_DATA ffd;

	memset(searchBuffer, 0, sizeof(searchBuffer));
	drunk_strcpy(searchBuffer, dir);
	drunk_strcat(searchBuffer, "\\*");
	
	hFind = FindFirstFileA(searchBuffer, &ffd);
	if (hFind == INVALID_HANDLE_VALUE) {
		return (NULL);
	}

	do {
		if (_stricmp(ffd.cFileName, binary) == 0)
		{
			snprintf(resultBuffer, sizeof(resultBuffer), "%s\\%s", dir, ffd.cFileName);
			FindClose(hFind);
			return (drunk_strdup(resultBuffer));
		}
	} while (FindNextFileA(hFind, &ffd) != 0);

	FindClose(hFind);
	return (NULL);
}

const char* searchBinary(const char* binName)
{
    size_t cnt;
    const char* bin_name_exe;
    const char* path;
    const char* result;
    const char** syspaths;

    //std::ifstream fcheck(binName);
    if ((binName[0] == '.' && binName[1] == '\\') || (binName[1] == ':' && binName[2] == '\\'))
    {
        if (_access(binName, 0))
            return (NULL);
        return (drunk_strdup(binName));
    }

    cnt = 0;
    path = drunk_getenv("Path");
    syspaths = drunk_strsplit(path, ';');
    printf("PATH = %s\n", path);

    if (strcmp(binName + strlen(binName) - strlen(".exe"), ".exe") != 0)
    {
        bin_name_exe = (char*)malloc(strlen(binName) + strlen(".exe") + 1);
        drunk_strcpy((char*)bin_name_exe, binName);
        drunk_strcat((char*)bin_name_exe, ".exe");
    }
    else {
        bin_name_exe = NULL;
    }

    while (syspaths[cnt] != NULL)
    {
        result = lookupBinaryInDir(bin_name_exe == NULL ? binName : bin_name_exe, syspaths[cnt]);
        if (result != NULL)
        {
            if (bin_name_exe != NULL)
                free((void*)bin_name_exe);
            return (result);
        }
        cnt++;
    }
    if (bin_name_exe != NULL)
        free((void*)bin_name_exe);
    return (NULL);
}

size_t CORE_process_invoke(const char *binaryName, const char *arguments, const char **buffer)
{
	NTSTATUS status;
	UNICODE_STRING uImage;
	UNICODE_STRING uNtImage;
	UNICODE_STRING uParams;
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters = NULL;
	HMODULE hNtdll;
	_C_NtCreateUserProcess CreateUserProcess;
	_C_RtlCreateProcessParametersEx CreateParams;
	_C_RtlInitUnicodeString InitUnicode;
	_C_RtlAllocateHeap AllocateHeap;
	_C_RtlFreeHeap FreeHeap;
	const char* nonUnicodePath;
	char* nonUnicodeNtPath;
	char* nonUnicodeParams;
	wchar_t* unicodePath;
	wchar_t* unicodeNtPath;
	wchar_t* unicodeParams;

	hNtdll = LoadLibraryA("ntdll.dll\0");
	CreateUserProcess = (_C_NtCreateUserProcess)(void*)GetProcAddress(hNtdll, "NtCreateUserProcess");
	CreateParams = (_C_RtlCreateProcessParametersEx)(void*)GetProcAddress(hNtdll, "RtlCreateProcessParametersEx");
	InitUnicode = (_C_RtlInitUnicodeString)(void*)GetProcAddress(hNtdll, "RtlInitUnicodeString");
	AllocateHeap = (_C_RtlAllocateHeap)(void*)GetProcAddress(hNtdll, "RtlAllocateHeap");
	FreeHeap = (_C_RtlFreeHeap)(void*)GetProcAddress(hNtdll, "RtlFreeHeap");

	nonUnicodePath = searchBinary(binaryName);
	if (nonUnicodePath == NULL)
	{
		*buffer = drunk_strdup(ERROR_BINARY);
		return (strlen(ERROR_BINARY));
	}

	nonUnicodeNtPath = (char*)malloc(sizeof(char) * (strlen("\\??\\") + strlen(nonUnicodePath)) + 1);
	nonUnicodeParams = (char*)malloc(sizeof(char) * (strlen(nonUnicodePath) + strlen(arguments)) + 2);
	if (nonUnicodePath != NULL && nonUnicodeNtPath != NULL && nonUnicodeParams != NULL)
	{
		drunk_strcpy(nonUnicodeNtPath, "\\??\\");
		drunk_strcat(nonUnicodeNtPath, nonUnicodePath);

		drunk_strcpy(nonUnicodeParams, nonUnicodePath);
		drunk_strcat(nonUnicodeParams, " ");
		drunk_strcat(nonUnicodeParams, arguments);

		// All strings used to create the process
		unicodePath = (wchar_t *)malloc(sizeof(wchar_t) * 4096);
		unicodeNtPath = (wchar_t *)malloc(sizeof(wchar_t) * 4096);
		unicodeParams = (wchar_t *)malloc(sizeof(wchar_t) * 4096);
		MultiByteToWideChar(CP_ACP, 0, nonUnicodePath, -1, unicodePath, 4096);
		MultiByteToWideChar(CP_ACP, 0, nonUnicodeNtPath, -1, unicodeNtPath, 4096);
		MultiByteToWideChar(CP_ACP, 0, nonUnicodeParams, -1, unicodeParams, 4096);
		InitUnicode(&uImage, unicodePath);
		InitUnicode(&uNtImage, unicodeNtPath);
		InitUnicode(&uParams, unicodeParams);
	}
	else {
		*buffer = drunk_strdup(ERROR_GENERIC);
		return (strlen(ERROR_GENERIC));
	}

	// Creates PIPE to read process output
	HANDLE pipeRead, pipeWrite;
	PSECURITY_ATTRIBUTES secAttr;

	// Don't know why, but CreatePipe() fails quite often here
	// Retrying by bruteforce until it works fixes the issue
	// Yes that's ugly, but hey, if it's stupid and it works it's not stupid
	int i = 0;
	for (i = 0; i < 50; i++)
	{
		secAttr = (PSECURITY_ATTRIBUTES)malloc(sizeof(SECURITY_ATTRIBUTES));
		secAttr->nLength = sizeof(SECURITY_ATTRIBUTES);
		secAttr->bInheritHandle = TRUE;

		if (CreatePipe(&pipeRead, &pipeWrite, secAttr, 0))
		    break;

		Sleep(10);
	}
	if (i == 50)
	{
		*buffer = drunk_strdup(ERROR_PIPE);
		return (strlen(ERROR_PIPE));
	}

	// Initialize process parameters
	status = CreateParams(&ProcessParameters, &uImage, NULL, NULL, &uParams, NULL, NULL, NULL, NULL, NULL, RTL_USER_PROCESS_PARAMETERS_NORMALIZED);
	//ProcessParameters->WindowFlags = WF_HIDE_WINDOW | WF_HIDE_TEXT;
	ProcessParameters->Flags = 0x6001;
	ProcessParameters->WindowFlags = WF_HIDE_WINDOW | WF_HIDE_TEXT | WF_REDIRECT_HANDLES;
	ProcessParameters->StandardInput = 0x00;
	ProcessParameters->StandardOutput = pipeWrite;
	ProcessParameters->StandardError = pipeWrite;

	// Initialize the PS_CREATE_INFO structure
	PS_CREATE_INFO CreateInfo = { 0 };
	CreateInfo.Size = sizeof(CreateInfo);
	CreateInfo.State = PsCreateInitialState;

	// Initialize the PS_ATTRIBUTE_LIST structure
	PPS_STD_HANDLE_INFO stdHandleInfo = (PPS_STD_HANDLE_INFO)AllocateHeap(
		UTILS_RtlGetThreadEnvironmentBlock()->ProcessEnvironmentBlock->ProcessHeap,
		HEAP_ZERO_MEMORY,
		sizeof(PS_STD_HANDLE_INFO)
	);
	stdHandleInfo->Flags = 0x00;
	PCLIENT_ID clientId = (PCLIENT_ID)AllocateHeap(
		UTILS_RtlGetThreadEnvironmentBlock()->ProcessEnvironmentBlock->ProcessHeap,
		HEAP_ZERO_MEMORY,
		sizeof(PS_ATTRIBUTE)
	);
	PSECTION_IMAGE_INFORMATION SecImgInfo = (PSECTION_IMAGE_INFORMATION)AllocateHeap(
		UTILS_RtlGetThreadEnvironmentBlock()->ProcessEnvironmentBlock->ProcessHeap,
		HEAP_ZERO_MEMORY,
		sizeof(SECTION_IMAGE_INFORMATION)
	);
	PPS_ATTRIBUTE_LIST AttributeList = (PS_ATTRIBUTE_LIST*)AllocateHeap(
		UTILS_RtlGetThreadEnvironmentBlock()->ProcessEnvironmentBlock->ProcessHeap,
		HEAP_ZERO_MEMORY,
		sizeof(PS_ATTRIBUTE_LIST)
	);

	// Create necessary attributes
	AttributeList->TotalLength = sizeof(PS_ATTRIBUTE_LIST);
	AttributeList->Attributes[0].Attribute = PS_ATTRIBUTE_CLIENT_ID;
	AttributeList->Attributes[0].Size = sizeof(CLIENT_ID);
	AttributeList->Attributes[0].ValuePtr = clientId;

	AttributeList->Attributes[1].Attribute = PS_ATTRIBUTE_IMAGE_INFO;
	AttributeList->Attributes[1].Size = sizeof(SECTION_IMAGE_INFORMATION);
	AttributeList->Attributes[1].ValuePtr = SecImgInfo;

	AttributeList->Attributes[2].Attribute = PS_ATTRIBUTE_IMAGE_NAME;
	AttributeList->Attributes[2].Size = uNtImage.Length;
	AttributeList->Attributes[2].ValuePtr = uNtImage.Buffer;

	AttributeList->Attributes[3].Attribute = 0x2000a;
	AttributeList->Attributes[3].Size = sizeof(PS_STD_HANDLE_INFO);
	AttributeList->Attributes[3].ValuePtr = stdHandleInfo;

	DWORD64 policy = PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON;
	// Add process mitigation attribute
	AttributeList->Attributes[4].Attribute = PS_ATTRIBUTE_MITIGATION_OPTIONS;
	AttributeList->Attributes[4].Size = sizeof(DWORD64);
	AttributeList->Attributes[4].ValuePtr = &policy;

	// No spoof PPID
	AttributeList->TotalLength -= sizeof(PS_ATTRIBUTE);

	// Create the process
	HANDLE hProcess = NULL, hThread = NULL;
	OBJECT_ATTRIBUTES processObjectAttributes;
	OBJECT_ATTRIBUTES threadObjectAttributes;
	InitializeObjectAttributes(&processObjectAttributes, NULL, 0, NULL, NULL);
	InitializeObjectAttributes(&threadObjectAttributes, NULL, 0, NULL, NULL);
	status = CreateUserProcess(
		&hProcess,
		&hThread,
		MAXIMUM_ALLOWED,
		MAXIMUM_ALLOWED,
		&processObjectAttributes, //processObjectAttributes
		&threadObjectAttributes, //threadObjectAttributes
		PROCESS_CREATE_FLAGS_INHERIT_HANDLES, //PROC_FLAGS
		0, //THREAD_FLAGS
		ProcessParameters, //parameters
		&CreateInfo,       //createInfo
		AttributeList      //attributeList
	);
	if (CreateInfo.State != PsCreateSuccess) {
		printf("[!] Failed to invoke process, handle = 0x%px (State = %d) (NTSTATUS = 0x%lx)\n", hProcess, CreateInfo.State, status);
		*buffer = drunk_strdup("");
		free(unicodePath);
		free(unicodeNtPath);
		free(unicodeParams);
		return (0);
	}

	// Prevent parent from writing to pipe
	CloseHandle(pipeWrite);

	{
		size_t totalBufferSize;
		char* finalBuffer, * tmpBuffer;
		totalBufferSize = 0;
		finalBuffer = NULL;
		tmpBuffer = NULL;
		while (1)
		{
			char buffer[100];
			DWORD bytesRead;

			bytesRead = 0;
			memset(buffer, 0, 100);
			if (!ReadFile(pipeRead, buffer, 100, &bytesRead, NULL) || bytesRead == 0) {
				break;
			}

			tmpBuffer = (char*)malloc(totalBufferSize + bytesRead + 1);
			memcpy(tmpBuffer, finalBuffer, totalBufferSize);
			memcpy(tmpBuffer + totalBufferSize, buffer, bytesRead);
			*(tmpBuffer + totalBufferSize + bytesRead) = '\0';
			if (finalBuffer != NULL) { free(finalBuffer); }
			finalBuffer = tmpBuffer;
			totalBufferSize += bytesRead;
		}
		*buffer = finalBuffer;

		// Clean up
		FreeHeap(UTILS_RtlGetThreadEnvironmentBlock()->ProcessEnvironmentBlock->ProcessHeap, 0, AttributeList);
		//RtlDestroyProcessParameters(ProcessParameters);

		return (totalBufferSize);
	}

	free(unicodePath);
	free(unicodeNtPath);
	free(unicodeParams);
	return ((size_t)-1);
}

