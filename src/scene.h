#pragma once
#include <vector>
#include <SDL/SDL.h>
#include <nanovg.h>
#include <algorithm>

#include "image.h"
#include "console/console.h"

class Scene {
public:
	virtual ~Scene() {};
	virtual void Startup(ClientInfo* i) = 0;
	virtual void Update(float dt) = 0;
	virtual void Render() = 0;
	virtual bool Event(SDL_Event *ev) {
		return true;
	};
};

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

	bool Event(SDL_Event *ev) {
		for (auto s = scenes.rbegin(); s != scenes.rend(); ++s) {
			if ((*s)->Event(ev) == false) {
				return false;
			}
		}

		return true;
	}

	void Push(Scene* newScene) {
		scenes.push_back(newScene);
		newScene->Startup(&info);
	}

	Scene* Get(unsigned int i) {
		if (i > scenes.size()) {
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