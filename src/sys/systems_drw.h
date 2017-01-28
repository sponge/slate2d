#pragma once
#include <entityx/entityx.h>
#include <tmx.h>
#include "../image.h"
#include "../components.h"

namespace ex = entityx;

struct RectDrawSystem : public ex::System<RectDrawSystem> {
	explicit RectDrawSystem(ClientInfo *inf) : inf(inf) {};
	void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
	ClientInfo *inf;
};

struct TileMapDrawSystem : public ex::System<TileMapDrawSystem> {
	explicit TileMapDrawSystem(ClientInfo *inf) : inf(inf) {};
	void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
	void draw_image(ClientInfo *inf, tmx_map * map, tmx_layer *layer, ex::ComponentHandle<Camera> &cam);
	void draw_tiles(ClientInfo *inf, tmx_map * map, tmx_layer *layer, ex::ComponentHandle<Camera> &cam);
	ClientInfo *inf;
};

struct CameraDrawSystem : public ex::System<CameraDrawSystem> {
	explicit CameraDrawSystem(ClientInfo *inf) : inf(inf) {};
	void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
	ClientInfo *inf;
};

struct SpriteDrawSystem : public ex::System<SpriteDrawSystem> {
	explicit SpriteDrawSystem(ClientInfo *inf) : inf(inf) {};
	void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
	ClientInfo *inf;
};