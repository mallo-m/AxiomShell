#include "Glibc.h"

static size_t count_words(char const* s, char c)
{
	size_t words;

	words = 0;
	while (*s)
	{
		while (*s == c)
			s++;
		if (*s)
		{
			words++;
			while (*s && *s != c)
				s++;
		}
	}
	return (words);
}

static const char* get_word(char* word, char c)
{
	char* start;

	start = word;
	while (*word && *word != c)
		word++;
	*word = '\0';
	return (drunk_strdup(start));
}

static const char** get_words(char* s, char c, size_t words_count)
{
	size_t i;
	const char* word;
	const char** words;

	i = 0;
	if ((words = (const char**)malloc((words_count + 1) * sizeof(char*))))
	{
		while (i < words_count)
		{
			while (*s == c)
				s++;
			if (*s)
			{
				if (!(word = get_word(s, c)))
					return (NULL);
				words[i++] = word;
				s += (strlen(word) + 1);
			}
		}
		words[i] = NULL;
	}
	return (words);
}

const char** drunk_strsplit(char const* s, char c)
{
	char* line;
	const char** words;

	if (!s || !(line = (char*)drunk_strdup(s)))
		return (NULL);
	words = get_words(line, c, count_words(line, c));
	free(line);
	return (words);
}

