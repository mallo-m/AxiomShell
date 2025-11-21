/*
* Parses option flags only
*
* Returns:
* -1 : Invalid option detected
* \d : How many arguments are to be skipped, as they hold options flags
*/

#include "AxiomShell.h"
#include "ls.h"
#include "Glibc.h"
#include "autoxor.h"

SIZE_T BUILTIN_LS_parse_options(int ac, char **av)
{
	STACK_RANDOMIZER;
	int i;
	size_t j;
	SIZE_T n;

	i = 0;
	g_ls_flags = LS_MODE_ONE;
	while (i < ac)
	{
		if (drunk_strcmp(av[i], "--") == 0)
			return (i + 1);

		if (*(av[i]) != '-')
			break;
		j = 1;
		while (av[i][j] != '\0')
		{
			if ((n = drunk_strrchr(XorStr("alRrtdG1Ss"), av[i][j])) == (SIZE_T)-1)
			{
				xprintf("[!] Illegal option: %c\n", av[i][j]);
				return (0);
			}
			g_ls_flags |= (1 << n);
			if (n == LsList)
				g_ls_flags &= ~LS_MODE_ONE;
			if (n == LsOne)
				g_ls_flags &= ~LS_MODE_LIST;
			j++;
		}
		i++;
	}
	return (i);
}

