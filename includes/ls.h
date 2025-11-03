#ifndef LS_H
# define LS_H

# include <windows.h>
# include <minwinbase.h>

enum LsOptions
{
	LsAll = 0,
	LsList = 1,
	LsRecurse = 2,
	LsReverse = 3,
	LsTimesort = 4,
	LsDirectory = 5,
	LsOne = 6,
	LsSizeSort = 7,
	LsHumanreadable = 8
};

# define LS_MODE_ALL (1UL<<LsAll)
# define LS_MODE_LIST (1UL<<LsList)
# define LS_MODE_RECURSE (1UL<<LsRecurse)
# define LS_MODE_REVERSE (1UL<<LsReverse)
# define LS_MODE_TIMESORT (1UL<<LsTimesort)
# define LS_MODE_DIRECTORY (1UL<<LsDirectory)
# define LS_MODE_ONE (1UL<<LsOne)
# define LS_SIZESORT (1UL<<LsSizeSort)
# define LS_HUMAN (1UL<<LsHumanReadable)

enum LsFiletype
{
	TypeRegularfile = 0,
	TypeDirectory = 1
};

typedef struct _LS_TARGET
{
	char *name;
	char *owner;
	enum LsFiletype type;
	SIZE_T size;
	SYSTEMTIME lastWriteTime;
	struct _LS_TARGET *next;
} LS_TARGET, *PLS_TARGET;

SIZE_T BUILTIN_LS_parse_options(int ac, char **av);
PLS_TARGET BUILTIN_LS_prepare_targets(SOCKET sock, size_t argc, SIZE_T options_cnt, char **argv);

void BUILTIN_LS_DISPLAY_TARGETS_regular_files(SOCKET sock, PLS_TARGET targets);
void BUILTIN_LS_DISPLAY_TARGETS_directories(SOCKET sock, PLS_TARGET targets);
void BUILTIN_LS_DISPLAY_folder_contents(SOCKET sock, char *folder_path);

extern unsigned int g_ls_flags;
extern unsigned int g_ls_hasdirectories;
extern unsigned int g_ls_hasfiles;

#endif


