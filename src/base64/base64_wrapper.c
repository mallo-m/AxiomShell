#include "Utils.h"
#include "base64.h"

char *WRAPPER_base64_encode(char *input)
{
	unsigned char *output;
	size_t input_len;
	size_t output_len;

	if (input == NULL)
		return (NULL);

	input_len = strlen(input);
	output_len = (size_t)((float)(input_len) * 1.5f) + 1;
	output = (unsigned char *)malloc(output_len);
	b64_encode((const unsigned char *)input, input_len, output);
	return ((char *)output);
}

char *WRAPPER_base64_encode_binary(char *input, size_t len)
{
	unsigned char *output;
	size_t output_len;

	if (input == NULL)
		return (NULL);

	output_len = (size_t)((float)(len) * 1.5f) + 1;
	output = (unsigned char *)malloc(output_len);
	b64_encode((const unsigned char *)input, len, output);
	return ((char *)output);
}

