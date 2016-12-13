#pragma once
#include <SDL/SDL.h>
#include <vector>
#include "scene.h"
#include "local.h"

class TestBounceWorld : public ex::EntityX {
public:
	explicit TestBounceWorld();
	void update(ex::TimeDelta dt);
};

class TestBounceScene : public Scene {
public:
	void Startup(ClientInfo* i) override;
	void Update(double dt) override;
	void Render() override;
	~TestBounceScene();

private:
	ClientInfo* inf;
	ex::SystemManager *rendSys;
	TestBounceWorld *world;
};