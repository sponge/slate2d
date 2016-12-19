#pragma once
#include <entityx/entityx.h>
#include <tmx.h>
#include "../local.h"

struct RectDrawSystem : public ex::System<RectDrawSystem> {
	explicit RectDrawSystem(ClientInfo *inf) : inf(inf) {};
	void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
	ClientInfo *inf;
};

struct TileMapDrawSystem : public ex::System<TileMapDrawSystem> {
	explicit TileMapDrawSystem(ClientInfo *inf) : inf(inf) {};
	void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
	void draw_image(ClientInfo *inf, tmx_map * map, tmx_layer *layer);
	void draw_tiles(ClientInfo *inf, tmx_map * map, tmx_layer *layer);
	ClientInfo *inf;
};

struct CameraDrawSystem : public ex::System<CameraDrawSystem> {
	explicit CameraDrawSystem(ClientInfo *inf) : inf(inf) {};
	void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
	ClientInfo *inf;
};