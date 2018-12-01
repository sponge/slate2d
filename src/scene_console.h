#pragma once
#include <vector>
#include <tmx.h>

#include "../game/shared.h"

class ConsoleScene : public Scene {
public:
	void Startup(ClientInfo* i) override;
	void Update(double dt) override;
	void Render() override;
	void Console(const char *) override {}; // not used for console

	bool consoleActive;

private:
	ClientInfo* inf;
};