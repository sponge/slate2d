#pragma once
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <imgui.h>
#include <ctype.h>
#include "../../game/shared.h"

#include <SDL/SDL_scancode.h>
#include <SDL/SDL_gamecontroller.h>

#include "../consoleng/console.h"

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
    void AddLog(const char* fmt, ...) IM_FMTARGS(2);
    void Draw(const char* title, bool* p_open);
    void ExecCommand(const char * command_line);
    static int TextEditCallbackStub(ImGuiTextEditCallbackData * data);
    int TextEditCallback(ImGuiTextEditCallbackData* data);
};

ConsoleUI* IMConsole();

// COMMAND SYSTEM

//void __cdecl Com_Error(int level, const char *error, ...);
//char *CopyString(const char *in);
//const char * __cdecl va(const char *format, ...);
//void Com_DefaultExtension(char *path, int maxSize, const char *extension);

// possibly private stuff that's still needed

// the game guarantees that no string from the network will ever
// exceed MAX_STRING_CHARS
#define	MAX_STRING_TOKENS	256		// max tokens resulting from Cmd_TokenizeString
#define MAX_QPATH 1024

#define NUM_MOUSE_BUTTONS 5
#define MOUSE_BUTTON_LEFT 0
#define MOUSE_BUTTON_RIGHT 1
#define MOUSE_BUTTON_MIDDLE 2
#define MOUSE_BUTTON_X1 3
#define MOUSE_BUTTON_X2 4

// BIND SYSTEM

typedef struct {
    bool		down;
    int			repeats;		// if > 1, it is autorepeating
    char		*binding;
} qkey_t;

#define MAX_CONTROLLERS 4
extern qkey_t		keys[SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * MAX_CONTROLLERS)];

void CL_InitKeyCommands(void);
int Key_StringToKeynum(const char *str);
const char *Key_KeynumToString(int keynum);