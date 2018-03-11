#pragma once
#include "shared.h"

class WrenScene : public Scene {
public:
	WrenScene(const char *mapFileName) : mapFileName(mapFileName) {};
	void Startup(ClientInfo* i) override;
	void Update(float dt) override;
	void Render() override;
	~WrenScene();

private:
	ClientInfo* inf;
	const char *mapFileName;
	struct WrenVM *vm;
};
