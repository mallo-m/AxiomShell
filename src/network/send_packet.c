#include "AxiomShell.h"
#include <ws2tcpip.h>

int NETWORK_send_packet(SOCKET sock, const char* packet_str, size_t _packet_len)
{
	STACK_RANDOMIZER;
	size_t packet_len;

	// Calculate what the length of the packet will be and send that
	if (_packet_len == (size_t)-1)
		packet_len = strlen(packet_str);
	else
		packet_len = _packet_len;

	// Send the actual packet's JSON
	return (send(sock, packet_str, packet_len, 0));
}

