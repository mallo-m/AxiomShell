#include "Utils.h"
#include "Typedefs.h"

//https://learn.microsoft.com/fr-fr/windows/win32/api/winternl/ns-winternl-teb
//Pointer to TEB of current thread (in this case the main one duh)
//This struct describes the state of a thread
//We will simply offset from TEB's address to get PEB's one
PTEB UTILS_RtlGetThreadEnvironmentBlock()
{
#if _WIN64
    return (PTEB)__readgsqword(0x30);
#else
    return (PTEB)__readfsdword(0x16);
#endif
}

