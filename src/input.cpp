#include "console\console.h"

bool KeyEvent(int key, bool down, unsigned time) {
	// send the bound action
	auto kb = keys[key].binding;
	char	cmd[1024];

	if (!kb) {
		return false;
	}
	else if (kb[0] == '+') {
		int i;
		char button[1024], *buttonPtr;
		buttonPtr = button;
		for (i = 0; ; i++) {
			if (kb[i] == ';' || !kb[i]) {
				*buttonPtr = '\0';
				if (button[0] == '+') {
					// button commands add keynum and time as parms so that multiple
					// sources can be discriminated and subframe corrected
					Com_sprintf(cmd, sizeof(cmd), "%s %i %i\n", button, key, time);
					Cbuf_AddText(cmd);
				}
				else {
					// down-only command
					Cbuf_AddText(button);
					Cbuf_AddText("\n");
				}
				buttonPtr = button;
				while ((kb[i] <= ' ' || kb[i] == ';') && kb[i] != 0) {
					i++;
				}
			}
			*buttonPtr++ = kb[i];
			if (!kb[i]) {
				break;
			}
		}
		return true;
	}
	else {
		// down-only command
		Cbuf_AddText(kb);
		Cbuf_AddText("\n");
		return true;
	}

	return false;
}