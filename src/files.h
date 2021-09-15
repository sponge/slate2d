#pragma once
#include "console.h"

extern conVar_t *fs_basepath;
extern conVar_t *fs_basegame;
extern conVar_t *fs_game;

void FS_Init(const char *argv0);
int FS_WriteFile(const char *filename, const void *data, int len);
int FS_ReadFile(const char *path, void **buffer);
bool FS_Exists(const char *file);
char** FS_List(const char *path);
void FS_FreeList(void * listVar);
const char *FS_FileExtension(const char *filename);