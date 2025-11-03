UTILS_PATH = ./src/utils/
UTILS = drunk_atoi.c			\
	drunk_strcpy.c			\
	drunk_strcat.c			\
	drunk_strdup.c			\
	drunk_strcmp.c			\
	drunk_strncmp.c			\
	drunk_strsplit.c		\
	drunk_strrchr.c			\
	drunk_getenv.c			\
	get_command_prompt.c		\
	get_impersonated_user_and_sid.c \
	TEB.c

UTILS_SRCS = $(addprefix $(UTILS_PATH),$(UTILS))
SRCS += $(UTILS_SRCS)

