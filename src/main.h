#pragma once

#define MAX_BUTTONS 12

extern bool consoleActive;

extern double com_frameTime, com_lastFrameTime, frame_msec;
extern kbutton_t buttons[12];

void DropToMenu();