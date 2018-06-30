#pragma once
#include "shared.h"

class WrenScene : public Scene {
public:
	WrenScene(const char *mainScriptName, const char *mapFileName) : mainScriptName(mainScriptName), mapFileName(mapFileName) {};
	void Startup(ClientInfo* i) override;
	void Update(float dt) override;
	void Render() override;
	~WrenScene();

	void Console(const char *str);

private:
	ClientInfo* inf;
	const char *mainScriptName;
	const char *mapFileName;
	struct WrenVM *vm;
};
