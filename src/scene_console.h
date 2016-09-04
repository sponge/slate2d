#pragma once
#include <SDL/SDL.h>
#include <vector>
#include "scene.h"
#include <tmx.h>

class ConsoleScene : public Scene {
public:
	void Startup(ClientInfo* i) override;
	void Teardown() override;
	void Update(double dt) override;
	void Render() override;
	bool Event(SDL_Event *ev) override;

private:
	ClientInfo* inf;
	bool consoleActive;
	char currentLine[MAX_CMD_LINE];
};