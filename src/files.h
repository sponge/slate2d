#pragma once
#include "console/console.h"

extern cvar_t *fs_basepath;
extern cvar_t *fs_basegame;
extern cvar_t *fs_game;

void FS_Init(const char *argv0);
int FS_ReadFile(const char *path, void **buffer);
bool FS_Exists(const char *file);
char** FS_List(const char *path);
void FS_FreeList(void * listVar);
const char *FS_FileExtension(const char *filename);