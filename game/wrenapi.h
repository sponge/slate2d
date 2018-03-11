#pragma once

struct WrenVM *Wren_Init();
void Wren_Frame(struct WrenVM *vm, float dt, int w, int h);

typedef struct {
	struct WrenHandle *instanceHnd, *updateHnd, *drawHnd;
} wrenHandles_t;