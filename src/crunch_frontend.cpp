#include "console/console.h"
#include <crunch.h>

void Cmd_Crunch_f() {
    const char *args[MAX_STRING_TOKENS];
    int argc = Con_GetArgsCount();
    for (int i = 0; i < argc; i++) {
        args[i] = Con_GetArg(i);
    }
    crunch_main(argc, args);
}

void Crunch_Init() {
    Con_AddCommand("crunch", &Cmd_Crunch_f);
}