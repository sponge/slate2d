#pragma once
#include <entityx/entityx.h>
#include "../local.h"

struct RectMoverSystem : public ex::System<RectMoverSystem> {
	explicit RectMoverSystem() {};
	void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
};

struct CameraUpdateSystem : public ex::System<CameraUpdateSystem> {
	explicit CameraUpdateSystem() {};
	void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
};

struct InputSystem : public ex::System<InputSystem> {
	explicit InputSystem() {};
	void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
};

struct PlayerSystem : public ex::System<PlayerSystem> {
	explicit PlayerSystem() {};
	void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
};