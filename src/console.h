#pragma once
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

// COMMAND SYSTEM

typedef unsigned char 		byte;
typedef void(*xcommand_t) (void);

void __cdecl Com_Error(int level, const char *error, ...);
char *CopyString(const char *in);

void Cbuf_Init(void);
void Cbuf_AddText(const char *text);
void Cbuf_InsertText(const char *text);
void Cbuf_ExecuteText(int exec_when, const char *text);
void Cbuf_Execute(void);

int	Cmd_Argc(void);
char *Cmd_Argv(int arg);
void Cmd_Init(void);
void Cmd_ExecuteString(const char *text);
void Cmd_AddCommand(const char *cmd_name, xcommand_t function);
void Cmd_RemoveCommand(const char *cmd_name);

// possibly private stuff that's still needed

#define Com_Printf printf
#define Com_DPrintf printf
#define Com_Memcpy memcpy
#define Com_sprintf snprintf

// the game guarantees that no string from the network will ever
// exceed MAX_STRING_CHARS
#define	MAX_STRING_CHARS	1024	// max length of a string passed to Cmd_TokenizeString
#define	MAX_STRING_TOKENS	256		// max tokens resulting from Cmd_TokenizeString
#define	MAX_TOKEN_CHARS		1024	// max length of an individual token

#define	MAX_INFO_STRING		1024
#define	MAX_INFO_KEY		1024
#define	MAX_INFO_VALUE		1024

#define	MAX_CMD_BUFFER	16384
#define	MAX_CMD_LINE	1024

#define	BIG_INFO_STRING		8192  // used for system info key only

typedef struct {
	byte	*data;
	int		maxsize;
	int		cursize;
} cmd_t;

// paramters for command buffer stuffing
typedef enum {
	EXEC_NOW,			// don't return until completed, a VM should NEVER use this,
						// because some commands might cause the VM to be unloaded...
						EXEC_INSERT,		// insert at current position, but don't run yet
						EXEC_APPEND			// add to end of the command buffer (normal case)
} cbufExec_t;

// parameters to the main Error routine
typedef enum {
	ERR_NONE,
	ERR_FATAL,					// exit the entire game with a popup window
	ERR_DROP,					// print to console and disconnect from game
	ERR_DISCONNECT,				// don't kill server
	ERR_NEED_CD					// pop up the need-cd dialog
} errorParm_t;

// CVARS

#define	CVAR_ARCHIVE		1	// set to cause it to be saved to vars.rc
// used for system variables, not for player
// specific configurations
#define	CVAR_USERINFO		2	// sent to server on connect or change
#define	CVAR_SERVERINFO		4	// sent in response to front end requests
#define	CVAR_SYSTEMINFO		8	// these cvars will be duplicated on all clients
#define	CVAR_INIT			16	// don't allow change from console at all,
// but can be set from the command line
#define	CVAR_LATCH			32	// will only change when C code next does
// a Cvar_Get(), so it can't be changed
// without proper initialization.  modified
// will be set, even though the value hasn't
// changed yet
#define	CVAR_ROM			64	// display only, cannot be set by user at all
#define	CVAR_USER_CREATED	128	// created by a set command
#define	CVAR_TEMP			256	// can be set even when cheats are disabled, but is not archived
#define CVAR_CHEAT			512	// can not be changed if cheats are disabled
#define CVAR_NORESTART		1024	// do not clear when a cvar_restart is issued

// nothing outside the Cvar_*() functions should modify these fields!
typedef struct cvar_s {
	char		*name;
	char		*string;
	char		*resetString;		// cvar_restart will reset to this value
	char		*latchedString;		// for CVAR_LATCH vars
	int			flags;
	bool	modified;			// set each time the cvar is changed
	int			modificationCount;	// incremented each time the cvar is changed
	float		value;				// atof( string )
	int			integer;			// atoi( string )
	struct cvar_s *next;
	struct cvar_s *hashNext;
} cvar_t;

#define	MAX_CVAR_VALUE_STRING	256

void Cvar_Init(void);
static cvar_t *Cvar_FindVar(const char *var_name);
float Cvar_VariableValue(const char *var_name);
int Cvar_VariableIntegerValue(const char *var_name);
char *Cvar_VariableString(const char *var_name);
void Cvar_VariableStringBuffer(const char *var_name, char *buffer, int bufsize);
cvar_t *Cvar_Get(const char *var_name, const char *var_value, int flags);
cvar_t *Cvar_Set2(const char *var_name, const char *value, bool force);
void Cvar_Set(const char *var_name, const char *value);
void Cvar_SetLatched(const char *var_name, const char *value);
void Cvar_SetValue(const char *var_name, float value);
void Cvar_Reset(const char *var_name);
bool Cvar_Command(void);