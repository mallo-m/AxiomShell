#include "AxiomShell.h"
#include "ls.h"
#include "winnt.h"

#include <windows.h>
#include <minwindef.h>
#include <stdio.h>

unsigned int g_ls_flags = 0;
unsigned int g_ls_hasdirectories = 0;
unsigned int g_ls_hasfiles = 0;

BOOL BUILTIN_ls(SOCKET sock, size_t argc, char **argv)
{
	SIZE_T options_cnt;
	PLS_TARGET targets;

	printf("[+] Running BUILTIN_LS\n");
	options_cnt = BUILTIN_LS_parse_options(argc, argv);
	printf("[+] Parsed %lld arguments, g_flags=0x%x\n", options_cnt, g_ls_flags);
	targets = BUILTIN_LS_prepare_targets(sock, argc, options_cnt, argv);
	if (targets == NULL)
	{
		printf("[!] Unexpected error while preparing targets\n");
		return (FALSE);
	}

	BUILTIN_LS_DISPLAY_TARGETS_regular_files(sock, targets);
	if (g_ls_hasfiles && g_ls_hasdirectories && (!(g_ls_flags & LS_MODE_DIRECTORY))) {
		JSON_send_packets(JsonCommandOutput, sock, (void*)"\n");
	}
	BUILTIN_LS_DISPLAY_TARGETS_directories(sock, targets);
	
	return (TRUE);
}

