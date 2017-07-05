#pragma once
#include <functional>

typedef std::function<bool()> TimerCallback;

typedef struct {
	unsigned int group;
	int id;
	unsigned int runTime;
	bool repeat;
	TimerCallback callback;
} TimerItem;