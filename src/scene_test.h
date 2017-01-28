#pragma once
#include <entityx/entityx.h>
#include <SDL/SDL.h>
#include <vector>
#include "scene.h"


class TestWorld : public entityx::EntityX {
public:
	explicit TestWorld();
	void update(entityx::TimeDelta dt);
};

class TestScene : public Scene {
public:
	void Startup(ClientInfo* i) override;
	void Update(float dt) override;
	void Render() override;
	~TestScene();

private:
	ClientInfo* inf;
	entityx::SystemManager *rendSys;
	TestWorld *world;
};