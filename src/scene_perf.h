#pragma once
#include <SDL/SDL.h>
#include "scene.h"
#include "perf/perf.h"

class PerfScene : public Scene {
public:
	void Startup(ClientInfo* i) override;
	void Teardown() override;
	void Update(double dt) override;
	void Render() override;

private:
	PerfGraph fps;
	ClientInfo* i;
};