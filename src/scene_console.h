#pragma once
#include <vector>
#include "scene.h"
#include <tmx.h>

class ConsoleScene : public Scene {
public:
	void Startup(ClientInfo* i) override;
	void Update(float dt) override;
	void Render() override;

private:
	ClientInfo* inf;
};