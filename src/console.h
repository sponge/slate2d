#pragma once

// console.h - provides quake style console parsing, commands, convars, and bind system to any application.

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "external/sds.h"
#include "external/vec.h"
#include "external/map.h"

// valid flags for conVar_t flags
#define CONVAR_ROM 1<<0  // value can never be changed and only set by code
#define CONVAR_STARTUP 1<<1 // value can only be set during startup
#define CONVAR_USER 1<<2 // value was set by user

// valid values for Con_Error
#define ERR_NONE 0 // not an error, will be ignored by error system
#define ERR_GAME 1 // game error, non-fatal, can be handled anyway by game, but usually dumps back to menu
#define ERR_FATAL 2 // unrecoverable error, should exit after shutting down as much as possible

// console variable type
typedef struct {
	sds name; // name of the var
	sds defaultValue; // default string value, specified by Con_GetVarDefault
	int flags; // bitmask of flags for var
	bool modified; // set to true when changed, can be set to false by anyone
	int modifiedCount; // how many times the var has been changed

	sds string; // string value of the var
	float value; // float value
	int integer; // integer value
	bool boolean; // boolean value
} conVar_t;

// buttons are commands that start with + and are registered through Con_AllocateButtons
// they are used for player actions, like "jump" or "shoot" or "left" and let you query
// the held state of the button, and for how long they've been held.
typedef struct {
	sds name; // name of the bind controlling this button
	bool held; // if the key is being held down at all
	int	keysHeld[8]; // which keys are holding this button down
	int64_t timestamp; // timestamp button was first held down
	bool wasPressed;	// set when down, can be manually cleared by user
} buttonState_t;

// console command handlers are void functions that receive no arguments. in order to query
// the console use functions like Con_GetArg.
typedef void(*conCmd_t) ();

// dynamic array/map types for console state
typedef map_t(conVar_t) conVar_map_t;
typedef map_t(conCmd_t) conCmd_map_t;
typedef vec_t(sds) sds_vec_t;
typedef vec_t(buttonState_t) buttonState_vec_t;

// event handlers for places where the console needs to interact with your application. these
// should be set on the conState_t you initialize, there is no need to allocate a conHandlers_t
// directly.
typedef struct {
	// custom error handler. if not specified, exit(1) is called for any error
	void(*error)(int level, const char *message); 
	// print a message to screen, log, or wherever else. if not specified, printf(message) is used
	void(*print)(const char *message);
	// called during console execution if a command isn't found. use Con_GetArg and friends
	// to get parameters and potentially handle it. return true if handled, false if not.
	bool(*unhandledCommand)();
	// if using input system, needs to return a human-readable form for the key number specified.
	const char *(*getStringForKey)(int key);
	// if using input system, needs to return a key number for a human-readable string.
	int(*getKeyForString)(const char *key);
} conHandlers_t;

// the main keeper of console state. this is the good stuff!
typedef struct {
	int argc; // number of args
	sds *argv; // array of sds strings for each parameter
	conVar_map_t vars; // map of var_name -> conVar_t for all known convars
	conCmd_map_t cmds; // map of command name -> void(void) function receiver
	sds_vec_t binds; // array of key num -> sds string containing command to run on key press
	buttonState_vec_t buttons; // array of buttons, which are commands preceded by a + and track held state
	conHandlers_t handlers; // see conHandlers_t, developer-specified handlers for various console events

	// temp state
	sds cmd; // full string of command currently running
	sds tempArgs; // temp storage used for functions that return a string
	const char **sargv; // used for commandling arg parsing
	int sargc; // used for commandling arg parsing
	char error[1024]; // fixed sized buffer to avoid mem allocation
} conState_t;

// initializes the console and sets it as the active one. a global is used in console.c to hold this
// pointer so it should remain valid for the lifetime of the console.
void Con_Init(conState_t *con);

// deallocates all structures and frees all memory used for console. will not free the base pointer
void Con_Shutdown();

// updates which instance of the console to use for all the following console related functions.
// there can only be one console active at a time, but swapping out console instances before calling
// a function will probably be fine!
void Con_SetActive(conState_t *newCon);

// throws an error, see ERR_ constants. should be handled by error function in conState_t.handlers
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
const char *Con_GetArg(int i);

// returns the number of arguments. will be 0 if command handling is finished.
int Con_GetArgsCount();

// returns a string containing all the args from "start" to the end.
// NOTE: the pointer returned will not be valid after command handling is finished,
// or if other console functions are called
const char *Con_GetArgs(int start);

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

// sets a convar to the given value, going through all validation. if the convar doesn't
// exist, a new one will be created, and will be flagged as CONVAR_USER. use Con_GetVarDefault
// if you want to manually specify flags (or not use any flags) for a new convar.
conVar_t * Con_SetVar(const char * name, const char * value);

// sets a convar to the given float value. shortcut function, see Con_SetVar.
conVar_t * Con_SetVarFloat(const char * name, float value);

// skips all validation and forces the value to be set. convar must already exist.
conVar_t * Con_SetVarForce(const char * name, const char * value);

// resets a convar to it's default value, using validation.
conVar_t * Con_ResetVar(const char * name);

// pass in the command line from startup to temporarily store convars and other commands
// so that they can be ran at a later time, after all initialization is complete. the typical flow
// is Con_SetupCommandLine -> as many Con_SetVarFromStartup as needed -> Con_ExecuteCommandLine once
// filesystem and any other prerequisites are setup enough so any commands should not fail.
void Con_SetupCommandLine(int argc, char *argv[]);

// find and set a convar from the stored commandline. this lets you pick out what you want to
// pull from the commandline without having to run it all. see Con_ParseCommandLine.
void Con_SetVarFromStartup(const char * name);

// execute everything that was stored in the command line storage. see Con_ParseCommandLine.
void Con_ExecuteCommandLine();

// allocate a given amount of keys that can be used to execute console commands. keys are referred
// to by number, ex something like SDL_NUM_SCANCODES.
void Con_AllocateKeys(int count);

// return the string that is bound to the given key number. you can use Con_HandleKeyPress if you just
// want to run that key's commands
const char *Con_GetBindForKey(int key);

// main entry point for input. will handle running the command, with special handling for button commands
// (commands starting with +) in order to track button state
void Con_HandleKeyPress(int key, bool down, int64_t time);

// returns a human-readable string for a given key. basically calls out to your handlers.getStringForKey
// function and returns the value
const char * Con_GetStringForKey(int key);

// returns an int for a human-readable string. basically calls out to your handlers.getKeyForString
// function and returns the value
int Con_GetKeyForString(const char *key);

// sets a console script to run when the given key is pressed. this is essentially what calling "bind"
// in the console calls.
void Con_SetBind(int key, const char *value);

// removes the script bound to that key, setting it back to NULL.
void Con_RemoveBind(int key);

// buttons are commands that start with +. these commands don't run a handler directly, instead they
// will update a corresponding structure with info like if this button is being held, which keys, and
// when the key started to be held. to use, pass in an array of strings of the names of the commands.
// for instance, ["left", "jump", "right", "shoot"]. then by binding a key to +shoot, and calling
// Con_GetButton(3) you will be able to determine if the user is holding down the +shoot button,
// no matter where the input is actually coming from: keyboard, mouse, gamepad, or however you are
// driving the input system.
void Con_AllocateButtons(const char ** buttonNames, int buttonCount);

// returns info corresponding to the button's state. the index here will be in the same order as the
// strings passed into Con_AllocateButtons.
buttonState_t * Con_GetButton(int buttonNum);

#ifdef __cplusplus
} // end extern
#endif