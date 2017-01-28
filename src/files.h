#pragma once

void FS_Init(const char *argv0);
int FS_ReadFile(const char *path, void **buffer);
bool FS_Exists(const char *file);
char** FS_List(const char *path);
void FS_FreeList(void * listVar);