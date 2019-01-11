#include "console/console.h"
#include <crunch.h>

void Cmd_Crunch_f() {
    const char *args[MAX_STRING_TOKENS];
    int argc = Cmd_Argc();
    for (int i = 0; i < argc; i++) {
        args[i] = Cmd_Argv(i);
    }
    crunch_main(argc, args);
}

void Crunch_Init() {
    Cmd_AddCommand("crunch", &Cmd_Crunch_f);
}