#pragma once
#include <entityx/entityx.h>
#include "../local.h"

struct RectMoverSystem : public ex::System<RectMoverSystem> {
	explicit RectMoverSystem() {};
	void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
};