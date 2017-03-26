#pragma once
#include <SDL/SDL.h>
#include <vector>
#include "../src/scene.h"
#include "baseworld.h"

class TestBounceWorld;

class TestBounceScene : public Scene {
public:
	void Startup(ClientInfo* i) override;
	void Update(float dt) override;
	void Render() override;
	~TestBounceScene();

private:
	ClientInfo* inf;
	BaseWorld *world;
};