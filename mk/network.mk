NETWORK_PATH = ./src/network/
NETWORK =	send_packet.c		\
		get_next_packet.c

NETWORK_SRCS = $(addprefix $(NETWORK_PATH),$(NETWORK))
SRCS += $(NETWORK_SRCS)

