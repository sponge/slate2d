#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include "shared.h"

const char * __cdecl va(const char *format, ...);
void Com_DefaultExtension(char *path, int maxSize, const char *extension);
#define MAX_KEYS 12
extern kbutton_t buttons[MAX_KEYS];