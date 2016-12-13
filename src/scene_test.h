#pragma once
#include <SDL/SDL.h>
#include <vector>
#include "scene.h"
#include "local.h"

class TestWorld : public ex::EntityX {
public:
	explicit TestWorld();
	void update(ex::TimeDelta dt);
};

class TestScene : public Scene {
public:
	void Startup(ClientInfo* i) override;
	void Update(double dt) override;
	void Render() override;

private:
	ClientInfo* inf;
	ex::SystemManager *rendSys;
	TestWorld *world;
};