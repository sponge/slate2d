#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <string>

const char * __cdecl va(const char *format, ...);
void Com_DefaultExtension(char *path, int maxSize, const char *extension);