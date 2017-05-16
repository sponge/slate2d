#pragma once
#include "shared.h"

class MenuScene : public Scene {
public:
	~MenuScene();
	void Startup(ClientInfo* i) override;
	void Update(float dt) override;
	void Render() override;

private:
	ClientInfo* inf;
	char **rawMaps;
	constexpr static int MAX_MAPS = 64;
	const char *maps[MAX_MAPS];
	int mapSize;
	int selected;
	bool showError;
};