#ifndef __REQUEST_HANDLER__PERFORMANCE_LOGGER__
#define __REQUEST_HANDLER__PERFORMANCE_LOGGER__

#include <memory>
#include <unordered_map>

class PerformanceLogger {
  public:
    typedef std::unordered_map<std::string, clock_t> TimestampMap;
    typedef std::unordered_map<std::string, float> DurationMap;

  public:
    void start(const std::string &action);
    void finish(const std::string &action);

    DurationMap::iterator begin() { return m_durations.begin(); }
    DurationMap::iterator end() { return m_durations.end(); }
    DurationMap::const_iterator cbegin() { return m_durations.cbegin(); }
    DurationMap::const_iterator cend() { return m_durations.cend(); }
  private:
    TimestampMap m_startTimes;
    DurationMap m_durations;
};


#endif // __REQUEST_HANDLER__PERFORMANCE_LOGGER__

