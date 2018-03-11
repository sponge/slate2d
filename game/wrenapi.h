#pragma once

struct WrenVM *Wren_Init();
void Wren_Update(WrenVM *vm, float dt);
void Wren_Draw(struct WrenVM *vm, int w, int h);

typedef struct {
	struct WrenHandle *instanceHnd, *updateHnd, *drawHnd;
} wrenHandles_t;