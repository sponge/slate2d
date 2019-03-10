#include "console.h"
#include <crunch.h>

void Cmd_Crunch_f() {
	extern conState_t console;

    crunch_main(console.argc, (const char **)console.argv);
}

void Crunch_Init() {
    Con_AddCommand("crunch", &Cmd_Crunch_f);
}