#ifndef __REQUEST_HANDLER__BASE__
#define __REQUEST_HANDLER__BASE__

#include <memory>
#include <ctime>

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/JSON/Object.h>

#include "../geo-index-registry.h"
#include "performance-logger.h"

class BaseRequestHandler : public Poco::Net::HTTPRequestHandler {
  public:
    BaseRequestHandler(const char *pName, GeoIndexRegistry &registry);
    ~BaseRequestHandler();

  protected:
    void SendResponse(Poco::Net::HTTPServerResponse &response, const Poco::JSON::Object::Ptr data);
    void BadRequest(Poco::Net::HTTPServerResponse &response);
    const Poco::JSON::Object::Ptr ReadRequestBody(Poco::Net::HTTPServerRequest &request);
    template <typename T> void Log(const std::string &key, const T &value);

  protected:
    std::string m_name;
    GeoIndexRegistry &m_registry;
    PerformanceLogger m_performanceLogger;
};

template <typename T>
void BaseRequestHandler::Log(const std::string &key, const T &value) {
#ifdef DEBUG
  std::cout << "[" << m_name << "/" << key << "]: ";
  std::cout << value << std::endl;
#endif // DEBUG
}

#endif // __REQUEST_HANDLER__BASE__
