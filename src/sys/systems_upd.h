#pragma once
#include <entityx/entityx.h>

struct RectMoverSystem : public entityx::System<RectMoverSystem> {
	explicit RectMoverSystem() {};
	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;
};

struct CameraUpdateSystem : public entityx::System<CameraUpdateSystem> {
	explicit CameraUpdateSystem() {};
	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;
};

struct InputSystem : public entityx::System<InputSystem> {
	explicit InputSystem() {};
	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;
};

struct PlayerSystem : public entityx::System<PlayerSystem> {
	explicit PlayerSystem() {};
	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;
};