#pragma once
#include "../src/scene.h"
#include "baseworld.h"

class GameMapScene : public Scene {
public:
	GameMapScene(const char *mapFileName) : mapFileName(mapFileName) {};
	void Startup(ClientInfo* i) override;
	void Update(float dt) override;
	void Render() override;
	~GameMapScene();

private:
	const char *mapFileName;
	ClientInfo* inf;
	BaseWorld *world;
};