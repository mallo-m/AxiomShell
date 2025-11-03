B64_PATH = ./src/base64/
B64 =	base64.c		\
	base64_wrapper.c

B64_SRCS = $(addprefix $(B64_PATH),$(B64))
SRCS += $(B64_SRCS)

