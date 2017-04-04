#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include "../src/input.h"

const char * __cdecl va(const char *format, ...);
void Com_DefaultExtension(char *path, int maxSize, const char *extension);
extern kbutton_t in_1_left, in_1_right, in_1_up, in_1_down, in_1_jump, in_1_attack, in_1_menu;