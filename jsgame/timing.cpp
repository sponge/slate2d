#include <chrono>
#include <imgui.h>
#include <string>
#include <unordered_map>
#include <vector>

using std::chrono::duration_cast;
using std::chrono::steady_clock;
using timerSample = std::pair<steady_clock::time_point, double>;

std::unordered_map<std::string, steady_clock::time_point> startTimes;
std::unordered_map<std::string, std::vector<timerSample>> timings;

void Timing_Start(const char *timerName)
{
  startTimes[std::string(timerName)] = steady_clock::now();
}

double Timing_End(const char *timerName)
{
  std::string name = std::string(timerName);
  auto endTime = steady_clock::now();
  auto ms = duration_cast<std::chrono::microseconds>(endTime - startTimes[name]);
  auto &history = timings[name];
  history.push_back({endTime, ms.count() / 1000.0});

  auto removeStart = std::remove_if(history.begin(), history.end(), [endTime](timerSample sample) {
    auto elapsed = duration_cast<std::chrono::milliseconds>(endTime - sample.first);
    return elapsed.count() >= 5000;
  });

  history.erase(removeStart, history.end());
  return ms.count() / 1000.0;
}

double Timer_Max(const char *timerName)
{
  auto &history = timings[std::string(timerName)];
  if (history.size() == 0) return 0;
  auto max = std::max_element(
    history.begin(), history.end(), [](const auto &lhs, const auto &rhs) { return lhs.second < rhs.second; });
  return max->second;
}