#pragma once

typedef struct {
	int			down[2];		// key nums holding it down
	unsigned	downtime;		// msec timestamp
	unsigned	msec;			// msec down this frame if both a down and up happened
	bool		active;			// current state
	bool		wasPressed;		// set when down, not cleared when up
} kbutton_t;

bool KeyEvent(int key, bool down, unsigned time);
void IN_KeyDown(kbutton_t *b);
void IN_KeyUp(kbutton_t *b);
float CL_KeyState(kbutton_t *key);