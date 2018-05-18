#include "performance-logger.h"

void PerformanceLogger::start(const std::string &action) {
  m_startTimes.insert(std::make_pair(action, clock()));
}

void PerformanceLogger::finish(const std::string &action) {
  clock_t startTime = m_startTimes[action];
  float duration = ((float)(clock() - startTime)) / CLOCKS_PER_SEC;

  m_durations.insert(std::make_pair(action, duration));
}

