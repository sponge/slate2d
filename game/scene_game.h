#pragma once
#include "shared.h"
#include "draw.h"

class GameScene : public Scene {
public:
	GameScene(const char *mapFileName) : mapFileName(mapFileName) {};
	void Startup(ClientInfo* i) override;
	void Update(float dt) override;
	void Render() override;
	~GameScene();

private:
	ClientInfo* inf;
	const char *mapFileName;
	AssetHandle dog, sprites, music, speech, font, spr;
	struct _tmx_map *map;
};