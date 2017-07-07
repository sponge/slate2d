#include "timer.h"
#include <list>
#include <functional>

std::list<TimerItem> timers;
static unsigned int timerId = 0;

int Timer_Create(unsigned int group, bool repeat, unsigned int runTime, TimerCallback callback) {
	auto timer = TimerItem();
	timer.callback = callback;
	timer.group = group;
	timer.id = timerId++;
	timer.repeat = repeat;
	timer.runTime = runTime;
	timers.push_back(timer);
	return timer.id;
}

bool Timer_Clear(unsigned int id) {
	timers.remove_if([id](TimerItem timer) { return timer.id == id; });
	return false;
}

void Timer_Tick(unsigned int group, unsigned int time) {
	for (auto timer = timers.begin(); timer != timers.end();) {
		if (timer->group != group) {
			continue;
		}

		if (time >= timer->runTime) {
			bool res = timer->callback();
			if (timer->repeat == false || res == false) {
				timers.erase(timer);
			}
			else {
				++timer;
			}
		}
	}
}