#pragma once
#include <SDL/SDL.h>
#include <vector>
#include "scene.h"
#include <tmx.h>

class MapScene : public Scene {
public:
	void Startup(ClientInfo* i) override;
	void Teardown() override;
	void Update(double dt) override;
	void Render() override;
	//bool Event(SDL_Event *ev) override;

private:
	ClientInfo* inf;
	EntityManager es;
	std::vector<System*> updateSystems;
	std::vector<RenderSystem*> renderSystems;
};