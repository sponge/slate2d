#pragma once

#include "../components.h"
#include "../pecs.h"

using namespace pecs;

struct TileMapDrawSystem : system_t {
	TileMapDrawSystem(ClientInfo *inf);
	void on_add(world_t *world) { }
	void on_remove(world_t *world) { }
	void on_add(entity_t *entity) { }
	void on_remove(entity_t *entity) { }
	void update(double dt);
	ClientInfo *inf;
};

struct CameraDrawSystem : system_t {
	CameraDrawSystem(ClientInfo *inf);
	void on_add(world_t *world) { }
	void on_remove(world_t *world) { }
	void on_add(entity_t *entity) { }
	void on_remove(entity_t *entity) { }
	void update(double dt);
	ClientInfo *inf;
};

struct RectDrawerSystem : system_t {
	RectDrawerSystem(ClientInfo *inf);
	void on_add(world_t *world) { }
	void on_remove(world_t *world) { }
	void on_add(entity_t *entity) { }
	void on_remove(entity_t *entity) { }
	void update(double dt);
	ClientInfo *inf;
};

struct SpriteDrawerSystem : system_t {
	SpriteDrawerSystem(ClientInfo *inf);
	void on_add(world_t *world) { }
	void on_remove(world_t *world) { }
	void on_add(entity_t *entity) { }
	void on_remove(entity_t *entity) { }
	void update(double dt);
	ClientInfo *inf;
};

struct PlayerInputSystem : system_t {
	PlayerInputSystem();
	void on_add(world_t *world) { }
	void on_remove(world_t *world) { }
	void on_add(entity_t *entity) { }
	void on_remove(entity_t *entity) { }
	void update(double dt);
};