CORE_PATH = ./src/core/
CORE =	main.c					\
	handle_command.c			\
	builtin_cd.c				\
	builtin_whoami.c			\
	builtin_cat.c				\
	builtin_rm.c				\
	builtin_rmdir.c				\
	builtin_mkdir.c				\
	builtin_download.c			\
	builtin_upload.c			\
	builtin_ls/ls_main.c			\
	builtin_ls/ls_parse_options.c		\
	builtin_ls/ls_prepare_targets.c 	\
	builtin_ls/ls_display_targets.c		\
	builtin_ls/ls_display_folder_contents.c	\
	process_invoker.c

CORE_SRCS = $(addprefix $(CORE_PATH),$(CORE))
SRCS += $(CORE_SRCS)

