JSON_PATH = ./src/json/
JSON =	json.c			\
	json_packets.cpp

JSON_SRCS = $(addprefix $(JSON_PATH),$(JSON))
SRCS += $(JSON_SRCS)

