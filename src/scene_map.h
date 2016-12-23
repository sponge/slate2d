#pragma once
#include <SDL/SDL.h>
#include <vector>
#include "scene.h"
#include "local.h"
#include <tmx.h>

class GameWorld : public ex::EntityX {
public:
	explicit GameWorld(const char *filename);
	void update(ex::TimeDelta dt);
	TileMap *tmap;
	~GameWorld();
};

class MapScene : public Scene {
public:
	MapScene(const char *filename);
	void Startup(ClientInfo* i) override;
	void Update(double dt) override;
	void Render() override;
	//bool Event(SDL_Event *ev) override;
	~MapScene();

private:
	const char *fileName;
	ClientInfo* inf;
	ex::SystemManager *rendSys;
	GameWorld *world;
};