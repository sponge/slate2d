#include <stdbool.h>
#include "../external/sds.h"
#include "../external/vec.h"
#include "../external/map.h"

typedef struct conVar {
    const char *name;

    const char *string;
    float value;
    int integer;
    bool boolean;
} conVar_t;

typedef void(*conCmd_t) ();

typedef map_t(conVar_t) conVar_map_t;
typedef map_t(conCmd_t) conCmd_map_t;

typedef struct conHandlers {
	void(*print)(const char *message);
	void(*unhandledCommand)();
} conHandlers_t;

typedef struct conState {
	unsigned int argc;
	sds *argv;
    conVar_map_t cvars;
    conCmd_map_t cmds;
	conHandlers_t handlers; 

	// internal state
	const char *cmd; // full command
	sds tempArgs; // temp storage used for functions that return a string
} conState_t;

conState_t *con;

// initializes the console and sets it as the active one.
void Con_Init(conState_t *con);

// updates which instance of the console to use.
void Con_SetActive(conState_t *newCon);

// parses and handles the string. this is the main entry point to using the
// console and is typically what you will want to pass user input to.
void Con_Execute(const char *cmd);

// calls printf if handler is not set
void Con_Print(const char *text);

// print wrapper, will call printf if a handler isn't set.
void Con_Printf(const char *fmt, ...);

// adds a new command handler "name" takes a void(void) function for callback
// if a command already exists, the callback is overriden. (FIXME: i think?)
void Con_AddCommand(const char *name, conCmd_t cb);

// removes the command handler "name"
void Con_RemoveCommand(const char *name);

// returns the argument at position i, if invalid, an empty string is returned
// NOTE: the pointer returned will not be valid after command handling is finished.
const char *Con_GetArg(unsigned int i);

// returns the number of arguments. will be 0 if command handling is finished.
int Con_GetArgsCount();

// returns a string containing all the args from "start" to the end.
// NOTE: the pointer returned will not be valid after command handling is finished,
// or if other console functions are called
const char *Con_GetArgs(unsigned int start);

// returns a string containing everything past the command
// useful if you just want to pass the whole set of arguments into somewhere else
const char *Con_GetRawArgs();