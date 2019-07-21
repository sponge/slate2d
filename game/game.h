#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <string>

#ifndef NOTUSED
	#define NOTUSED(v) for (;;) { (void)(1 ? (void)0 : ( (void)(v) ) ); break; }
#endif

const char* __cdecl gtempstr(const char* format, ...);
void Com_DefaultExtension(char *path, int maxSize, const char *extension);