#pragma once

#define MAX_BUTTONS 12

extern bool consoleActive;

extern unsigned com_frameTime;
extern unsigned frame_msec;
extern kbutton_t buttons[12];

void DropToMenu();