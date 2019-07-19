#pragma once
#include <cstdint>

#include "shared.h"

#ifndef NOTUSED
#define NOTUSED(v) for (;;) { (void)(1 ? (void)0 : ( (void)(v) ) ); break; }
#endif

extern bool consoleActive;

extern int64_t last_update_musec, com_frameTime, frame_musec;

void DropToMenu();
void SetWindowTitle(const char *title);