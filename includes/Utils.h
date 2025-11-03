#ifndef UTILS_H
# define UTILS_H

# include "Typedefs.h"
# include <tchar.h>

char* UTILS_get_command_prompt();
BOOL UTILS_get_impersonated_user_and_sid(char** domain, char** username, char** sid);
PTEB UTILS_RtlGetThreadEnvironmentBlock();
char *WRAPPER_base64_encode(char *input);
char *WRAPPER_base64_encode_binary(char *input, size_t len);

#endif

