#pragma once
#include <entityx/entityx.h>
#include <SDL/SDL.h>
#include <vector>
#include "scene.h"

class TestBounceWorld : public entityx::EntityX {
public:
	explicit TestBounceWorld();
	void update(entityx::TimeDelta dt);
};

class TestBounceScene : public Scene {
public:
	void Startup(ClientInfo* i) override;
	void Update(float dt) override;
	void Render() override;
	~TestBounceScene();

private:
	ClientInfo* inf;
	entityx::SystemManager *rendSys;
	TestBounceWorld *world;
};