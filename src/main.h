#pragma once
#include <cstdint>

#define MAX_BUTTONS 12

extern bool consoleActive;

extern int64_t com_frameTime, frame_musec;
extern kbutton_t buttons[12];

void DropToMenu();