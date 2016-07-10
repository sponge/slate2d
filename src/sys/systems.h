#pragma once
#include "../local.h"

struct RectDrawSystem : RenderSystem {
	void render(EntityManager &es, ClientInfo *inf);
};

struct RectMoverSystem : System {
	void update(EntityManager &es, double dt);
};
