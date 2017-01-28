#pragma once

typedef struct {
	int			down[2];		// key nums holding it down
	unsigned	downtime;		// msec timestamp
	unsigned	msec;			// msec down this frame if both a down and up happened
	bool		active;			// current state
	bool		wasPressed;		// set when down, not cleared when up
} kbutton_t;

extern kbutton_t in_1_left, in_1_right, in_1_up, in_1_down, in_1_jump, in_1_attack, in_1_menu;

bool KeyEvent(int key, bool down, unsigned time);
void IN_KeyDown(kbutton_t *b);
void IN_KeyUp(kbutton_t *b);
void CL_InitInput(void);
float CL_KeyState(kbutton_t *key);