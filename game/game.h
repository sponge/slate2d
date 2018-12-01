#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include "shared.h"

#ifndef NOTUSED
	#define NOTUSED(v) for (;;) { (void)(1 ? (void)0 : ( (void)(v) ) ); break; }
#endif

const char * __cdecl va(const char *format, ...);
void Com_DefaultExtension(char *path, int maxSize, const char *extension);
#define MAX_KEYS 12
extern kbutton_t buttons[MAX_KEYS];