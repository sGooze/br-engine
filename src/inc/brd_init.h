// brd_init.h - initialization procedures

#ifndef __BRD_INIT
#define __BRD_INIT
#include "brd_shared.h"

// IDEA: flag as a structure containing a pointer to the function to be executed if said flag is present (or is not present)

//bool    BRD_CheckFlag(const char*);

bool    BRD_Init(int, char**);
bool    BRD_InitCvar();

void    BRD_Shutdown();

extern  bool    flag_novid;

#endif // __BRD_INIT
