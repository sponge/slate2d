#pragma once
#include <SDL/SDL.h>
#include "scene.h"
#include "local.h"

class GLScene : public Scene {
public:
	void Startup(ClientInfo* i) override;
	void Update(float dt) override;
	void Render() override;
	~GLScene();

private:
	ClientInfo* inf;
	float _dt;
};
