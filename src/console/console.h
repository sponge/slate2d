#pragma once
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <imgui.h>
#include <ctype.h>
#include "../../game/shared.h"

// IMGUI CONSOLE

struct ConsoleUI
{
    char                  InputBuf[256];
    ImVector<char*>       Items;
    bool                  ScrollToBottom;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImVector<const char*> candidates;

    ConsoleUI();
    ~ConsoleUI();

    static int   Stricmp(const char* str1, const char* str2) { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
    static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
    static char* Strdup(const char *str) { size_t len = strlen(str) + 1; void* buff = malloc(len); return (char*)memcpy(buff, (const void*)str, len); }

    void ClearLog();
    void AddLog(const char* fmt, ...) IM_PRINTFARGS(2);
    void Draw(const char* title, bool* p_open);
    void ExecCommand(const char * command_line);
    static int TextEditCallbackStub(ImGuiTextEditCallbackData * data);
    int TextEditCallback(ImGuiTextEditCallbackData* data);
};

ConsoleUI* Console();

// COMMAND SYSTEM

void __cdecl Com_Error(int level, const char *error, ...);
char *CopyString(const char *in);
const char * __cdecl va(const char *format, ...);
void Com_DefaultExtension(char *path, int maxSize, const char *extension);

void Cbuf_Init(void);
void Cbuf_AddText(const char *text);
void Cbuf_InsertText(const char *text);
void Cbuf_ExecuteText(int exec_when, const char *text);
void Cbuf_Execute(void);

int	Cmd_Argc(void);
const char *Cmd_Argv(int arg);
const char *Cmd_ArgsFrom(int arg);
const char *Cmd_Cmd();
void Cmd_Init(void);
void Cmd_ExecuteString(const char *text);
void Cmd_AddCommand(const char *cmd_name, xcommand_t function);
void Cmd_RemoveCommand(const char *cmd_name);
void Com_ParseCommandLine(char *commandLine);
void Com_StartupVariable(const char *match);
bool Com_AddStartupCommands(void);
void Cmd_TokenizeString(const char *text_in);
void Cmd_CommandCompletion(void(*callback)(const char *match, const char *candidate), const char *match);
void Cvar_CommandCompletion(void(*callback)(const char *match, const char *candidate), const char *match);

// possibly private stuff that's still needed

#define Com_DPrintf Com_Printf
#define Com_Memcpy memcpy
#define Com_sprintf snprintf

void Com_Printf(const char *fmt, ...);

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
#define MAX_QPATH 1024

#define NUM_MOUSE_BUTTONS 5
#define MOUSE_BUTTON_LEFT 0
#define MOUSE_BUTTON_RIGHT 1
#define MOUSE_BUTTON_MIDDLE 2
#define MOUSE_BUTTON_X1 3
#define MOUSE_BUTTON_X2 4

void Cvar_Init(void);
cvar_t *Cvar_FindVar(const char *var_name);
float Cvar_VariableValue(const char *var_name);
int Cvar_VariableIntegerValue(const char *var_name);
const char *Cvar_VariableString(const char *var_name);
void Cvar_VariableStringBuffer(const char *var_name, char *buffer, int bufsize);
cvar_t *Cvar_Get(const char *var_name, const char *var_value, int flags);
cvar_t *Cvar_Set2(const char *var_name, const char *value, bool force);
void Cvar_Set(const char *var_name, const char *value);
void Cvar_SetLatched(const char *var_name, const char *value);
void Cvar_SetValue(const char *var_name, float value);
void Cvar_Reset(const char *var_name);
bool Cvar_Command(void);

// BIND SYSTEM

typedef struct {
    bool		down;
    int			repeats;		// if > 1, it is autorepeating
    char		*binding;
} qkey_t;

extern qkey_t		keys[512 + NUM_MOUSE_BUTTONS + (15 * 1)];

void CL_InitKeyCommands(void);
int Key_StringToKeynum(const char *str);
const char *Key_KeynumToString(int keynum);