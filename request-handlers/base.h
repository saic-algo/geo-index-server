#ifndef __REQUEST_HANDLER__BASE__
#define __REQUEST_HANDLER__BASE__

#include <memory>
#include <ctime>
#include <unordered_map>
#include <iostream>
#include <Poco/Net/HTTPRequestHandler.h>
#include "../geo-index.h"

class PerformanceLogger {
  public:
    typedef std::unordered_map<std::string, clock_t> TimestampMap;
    typedef std::unordered_map<std::string, float> DurationMap;
  public:
    void start(const std::string &action) {
      m_startTimes.insert(std::make_pair(action, clock()));
    }

    void finish(const std::string &action) {
      clock_t startTime = m_startTimes[action];
      float duration = ((float)(clock() - startTime)) / CLOCKS_PER_SEC;

      m_durations.insert(std::make_pair(action, duration));
    }

    DurationMap::iterator begin() { return m_durations.begin(); }
    DurationMap::iterator end() { return m_durations.end(); }
    DurationMap::const_iterator cbegin() { return m_durations.cbegin(); }
    DurationMap::const_iterator cend() { return m_durations.cend(); }
  private:
    TimestampMap m_startTimes;
    DurationMap m_durations;
};

class BaseRequestHandler : public Poco::Net::HTTPRequestHandler {
  public:
    BaseRequestHandler(std::shared_ptr<GeoIndexRegistry> registry)
      : m_registry(registry) { };

  protected:
    std::shared_ptr<GeoIndexRegistry> m_registry;
    PerformanceLogger m_performanceLogger;
};

#endif // __REQUEST_HANDLER__BASE__
