#pragma once
#include <vector>
#include <tmx.h>

#include "shared.h"

class ConsoleScene : public Scene {
public:
	void Startup(ClientInfo* i) override;
	bool Update(double dt) override;
	void Render() override;
	void Console(const char *) override {}; // not used for console

	bool consoleActive = false;

private:
	ClientInfo* inf = nullptr;
};