#pragma once

struct WrenVM *Wren_Init(const char *constructorStr);
void Wren_Update(WrenVM *vm, float dt);
void Wren_Draw(struct WrenVM *vm, int w, int h);
void Wren_Scene_Shutdown(WrenVM *vm);
void Wren_FreeVM(WrenVM *vm);

typedef struct {
	struct WrenHandle *instanceHnd, *updateHnd, *drawHnd, *shutdownHnd;
} wrenHandles_t;