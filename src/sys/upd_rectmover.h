#pragma once
#include "../local.h"

struct RectMoverSystem : System {
	virtual void update(EntityManager &es, double dt) = 0;
};