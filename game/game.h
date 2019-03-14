#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include "shared.h"

#ifndef NOTUSED
	#define NOTUSED(v) for (;;) { (void)(1 ? (void)0 : ( (void)(v) ) ); break; }
#endif

void Com_DefaultExtension(char *path, int maxSize, const char *extension);