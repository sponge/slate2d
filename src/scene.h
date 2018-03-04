#pragma once
#include <vector>
#include <nanovg.h>
#include <algorithm>

#include "console/console.h"
#include "../game/shared.h"

class SceneManager {
private:
	std::vector<Scene*> scenes;
	ClientInfo info;
public:
	SceneManager(ClientInfo i) {
		info = i;
	}

	void Switch(Scene* newScene) {
		for (auto s : scenes) {
			delete s;
		}
		scenes.clear();
		Push(newScene);
	}

	void Update(float dt) {
		for (auto s : scenes) {
			s->Update(dt);
		}
	}

	void Render() {
		for (auto s : scenes) {
			s->Render();
			nvgReset(info.nvg);
		}
	}

	void Push(Scene* newScene) {
		scenes.push_back(newScene);
		newScene->Startup(&info);
	}

	Scene* Get(unsigned int i) {
		if (i >= scenes.size()) {
			return nullptr;
		}
		return scenes.at(i);
	}

	void Replace(Scene * oldScene, Scene *newScene) {
		std::replace(scenes.begin(), scenes.end(), oldScene, newScene);
		delete oldScene;
		newScene->Startup(&info);
	}

	void Replace(int i, Scene *newScene) {
		auto oldScene = Get(i);
		if (oldScene == nullptr) {
			Switch(newScene);
			return;
		}

		Replace(oldScene, newScene);
	}

	void Pop() {
		if (scenes.size() == 0) {
			return;
		}

		auto lastScene = scenes.back();
		scenes.pop_back();
		delete lastScene;
	}

	void Clear() {
		scenes.clear();
	}

	Scene* Current() {
		if (scenes.size() == 0) {
			return nullptr;
		}
		
		return scenes.back();
	}
};