#pragma once
#include <list>
#include <SDL/SDL.h>
#include <nanovg.h>
#include "local.h"

class Scene {
public:
	virtual void Startup(ClientInfo* i) = 0;
	virtual void Teardown() = 0;
	virtual void Update(double dt) = 0;
	virtual void Render() = 0;
	virtual bool Event(SDL_Event *ev) {
		return true;
	};
};

class SceneManager {
private:
	std::list<Scene*> scenes;
	ClientInfo info;
public:
	SceneManager(ClientInfo i) {
		info = i;
	}

	void Switch(Scene* newScene) {
		for (auto s : scenes) {
			s->Teardown();
		}
		scenes.clear();
		Push(newScene);
	}

	void Update(double dt) {
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

	void Event(SDL_Event *ev) {
		for (auto s : scenes) {
			if (s->Event(ev) == false) {
				break;
			}
		}
	}

	void Push(Scene* newScene) {
		scenes.push_back(newScene);
		newScene->Startup(&info);
	}

	void Replace(Scene * oldScene, Scene *newScene) {
		oldScene->Teardown();
		std::replace(scenes.begin(), scenes.end(), oldScene, newScene);
		newScene->Startup(&info);
	}

	void Pop() {
		if (scenes.size() == 0) {
			return;
		}

		auto lastScene = scenes.back();
		scenes.pop_back();
		lastScene->Teardown();
	}

	Scene* Current() {
		return scenes.back();
	}
};