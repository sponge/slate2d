#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "external/sds.h"
#include "external/vec.h"
#include "external/map.h"

#define CONVAR_ROM 1<<0  // value can never be changed and only set by code
#define CONVAR_STARTUP 1<<1 // value can only be set during startup
#define CONVAR_USER 1<<2 // value was set by user

#define ERR_NONE 0
#define ERR_GAME 1
#define ERR_FATAL 2

typedef struct conVar {
    sds name; // name of the var
	sds defaultValue;
	int flags;
	bool modified; // set to true when changed, can be set to false by anyone
	int modifiedCount;

    sds string; // string value of the var
    float value; // float value
    int integer; // integer value
    bool boolean; // boolean value
} conVar_t;

typedef struct buttonState {
	sds name; // name of the bind controlling this button
	bool held; // if the key is being held down at all
	int	keysHeld[8]; // which keys are holding this button down
	int64_t timestamp; // timestamp button was first held down
	bool wasPressed;	// set when down, can be manually cleared by user
} buttonState_t;

typedef void(*conCmd_t) ();

typedef map_t(conVar_t) conVar_map_t;
typedef map_t(conCmd_t) conCmd_map_t;
typedef vec_t(sds) sds_vec_t;
typedef vec_t(buttonState_t) buttonState_vec_t;

typedef struct conHandlers {
	void(*error)(int level, const char *message);
	void(*print)(const char *message);
	bool(*unhandledCommand)();
	const char *(*getStringForKey)(int key);
	int(*getKeyForString)(const char *key);
} conHandlers_t;

typedef struct conState {
	unsigned int argc;
	sds *argv;
    conVar_map_t vars;
    conCmd_map_t cmds;
	sds_vec_t binds;
	buttonState_vec_t buttons;
	conHandlers_t handlers; 

	// internal state
	sds cmd; // full command
	sds tempArgs; // temp storage used for functions that return a string
	sds *sargv; // used for startup parsing and storing
	int sargc;
	char error[1024]; // fixed sized buffer to avoid mem allocation
} conState_t;

// initializes the console and sets it as the active one.
void Con_Init(conState_t *con);

// updates which instance of the console to use.
void Con_SetActive(conState_t *newCon);

void Con_Error(int level, const char * fmt, ...);

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

// finds and returns the named convar, returns NULL if not found
conVar_t *Con_GetVar(const char *name);

// finds and returns the named convar, returns a new var set to defaultValue if not found
// use this function to create new convars.
conVar_t *Con_GetVarDefault(const char *name, const char *defaultValue, int flags);

// returns the string value of a convar. returns an empty string if no convar exists.
const char * Con_GetVarString(const char * name);

// returns the float value of a convar. returns 0 if no convar exists.
float Con_GetVarFloat(const char * name);

// returns the int value of a convar. returns 0 if no convar exists.
int Con_GetVarInt(const char * name);

// returns the boolean value of a convar. returns false if no convar exists.
bool Con_GetVarBool(const char * name);

// sets a variable to the given value, going through all validation. if the cvar doesn't
// exist, a new one will be created, and will be flagged as CONVAR_USER. use Con_GetVarDefault
// if you want to specify flags for a new cvar
conVar_t * Con_SetVar(const char * name, const char * value);

conVar_t * Con_SetVarFloat(const char * name, float value);

conVar_t * Con_SetVarForce(const char * name, const char * value);

conVar_t * Con_ResetVar(const char * name);

void Con_ParseCommandLine(const char *cmdline);

void Con_ExecuteCommandLine();

void Con_SetVarFromStartup(const char * name);

void Con_AllocateKeys(int count);

const char *Con_GetBindForKey(int key);

void Con_HandleKeyPress(int key, bool down, int64_t time);

const char * Con_GetStringForKey(int key);

int Con_GetKeyForString(const char *key);

void Con_SetBind(int key, const char *value);

void Con_RemoveBind(int key);

void Con_AllocateButtons(const char ** buttonNames, int buttonCount);

buttonState_t * Con_GetButton(int buttonNum);

#ifdef __cplusplus
} // end extern
#endif