#include "AxiomShell.h"
#include "Glibc.h"

size_t packet_pool_len = 0;
unsigned char *packet_pool = NULL;

unsigned char* NETWORK_get_next_packet(SOCKET sock, int* bytesReceived)
{
	size_t i;
	size_t received_len;
	char buffer[4096];
	unsigned char* tmp_buf;

	if (packet_pool == NULL) { packet_pool = (unsigned char*)drunk_strdup(""); }
	while (1)
	{
		BOOL found = FALSE;

		for (i = 0; i < packet_pool_len; i++)
		{
			if (packet_pool[i] == '}')
			{
			found = TRUE;
			break;
			}
		}

		if (!found)
		{
			memset(buffer, 0, 4096);
			received_len = recv(sock, buffer, 4096, 0);
			tmp_buf = (unsigned char*)malloc(packet_pool_len + received_len);
			memcpy(tmp_buf, packet_pool, packet_pool_len);
			memcpy(tmp_buf + packet_pool_len, buffer, received_len);
			free(packet_pool);
			packet_pool = tmp_buf;
			packet_pool_len += received_len;
		}
		else
		{
			tmp_buf = (unsigned char*)malloc(i + 1 + 1);
			memcpy(tmp_buf, packet_pool, i + 1);
			tmp_buf[i + 1] = '\0';

			memcpy(packet_pool, packet_pool + i + 1, packet_pool_len - i - 1);
			packet_pool_len -= (i + 1);

			return (tmp_buf);
		}
	}

	return (NULL);
}

