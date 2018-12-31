#pragma once
#include <cstdint>

#ifndef NOTUSED
#define NOTUSED(v) for (;;) { (void)(1 ? (void)0 : ( (void)(v) ) ); break; }
#endif

#define MAX_BUTTONS 12

extern bool consoleActive;

extern int64_t com_frameTime, frame_musec;
extern kbutton_t buttons[12];

void DropToMenu();
void SetWindowTitle(const char *title);