#pragma once
#include "shared.h"

class WrenScene : public Scene {
public:
	WrenScene(const char *mainScriptName, const char *mapFileName) : mainScriptName(mainScriptName), mapFileName(mapFileName) {};
	void Startup(ClientInfo* i) override;
	void Update(double dt) override;
	void Render() override;
	~WrenScene();

	void Console(const char *str);
	ClientInfo* inf;

private:
	const char *mainScriptName;
	const char *mapFileName;
	struct WrenVM *vm;
	bool initialized = false;
};
