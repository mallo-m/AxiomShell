#include <windows.h>
#include <sddl.h>
#include <stdio.h>
#include "Glibc.h"
#include "Utils.h"

// This function accounts for impersonated tokens
BOOL UTILS_get_impersonated_user_and_sid(char** domain, char** username, char** sid)
{
	DWORD infoLength;
	PTOKEN_USER tokenUser;
	SID_NAME_USE sidType;
	LPSTR strSid;
	CHAR sName[256], sDomain[256];
	DWORD dwSizeName = sizeof(sName) / sizeof(sName[0]);
	DWORD dwSizeDomain = sizeof(sDomain) / sizeof(sDomain[0]);
	HANDLE threadToken;

	threadToken = NULL;
	if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &threadToken))
	{
		DWORD dwError;

		dwError = GetLastError();
		if (dwError == ERROR_NO_TOKEN)
		{
			GetTokenInformation(
				GetCurrentProcessToken(),
				TokenUser,
				NULL,
				0,
				&infoLength
			);
			tokenUser = (PTOKEN_USER)malloc(infoLength);
			GetTokenInformation(
				GetCurrentProcessToken(),
				TokenUser,
				tokenUser,
				infoLength,
				&infoLength
			);
		}
		else {
			printf("Unknown error\n");
			return (FALSE);
		}
	}
	else
	{
		GetTokenInformation(
				threadToken,
				TokenUser,
				NULL,
				0,
				&infoLength
		);
		tokenUser = (PTOKEN_USER)malloc(infoLength);
		GetTokenInformation(
			threadToken,
			TokenUser,
			tokenUser,
			infoLength,
			&infoLength
		);
	}

	if (LookupAccountSidA(NULL, tokenUser->User.Sid, sName, &dwSizeName, sDomain, &dwSizeDomain, &sidType))
	{
		ConvertSidToStringSidA(tokenUser->User.Sid, &strSid);
		if (domain != NULL)
			*domain = (char*)drunk_strdup(sDomain);
		if (username != NULL)
			*username = (char*)drunk_strdup(sName);
		if (sid != NULL)
			*sid = (char*)drunk_strdup(strSid);
		printf("%s - %s (%s)\n", sName, sDomain, strSid);
		free(tokenUser);
		return (TRUE);
	}

	free(tokenUser);
	return (FALSE);
}

