#pragma once
#include <vector>
#include "scene.h"
#include <tmx.h>

class ConsoleScene : public Scene {
public:
	void Startup(ClientInfo* i) override;
	void Update(double dt) override;
	void Render() override;
	void Console(const char *line) override {}; // not used for console

	bool consoleActive;

private:
	ClientInfo* inf;
};