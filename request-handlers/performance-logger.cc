#include "performance-logger.h"
#include "omp.h"

void PerformanceLogger::start(const std::string &action) {
  m_startTimes.insert(std::make_pair(action, omp_get_wtime()));
}

void PerformanceLogger::finish(const std::string &action) {
  float startTime = m_startTimes[action];
  float duration = omp_get_wtime() - startTime;

  m_durations.insert(std::make_pair(action, duration));
}

