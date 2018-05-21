#ifndef __REQUEST_HANDLER__BASE__
#define __REQUEST_HANDLER__BASE__

#include <memory>
#include <ctime>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/JSON/Object.h>
#include <Poco/Redis/Client.h>
#include "../geo-index.h"
#include "performance-logger.h"

using Poco::Redis::Client;


class BaseRequestHandler : public Poco::Net::HTTPRequestHandler {
  public:
    BaseRequestHandler(const char *pName, std::shared_ptr<GeoIndexRegistry> registry, std::shared_ptr<Client> client);

  protected:
    void SendResponse(Poco::Net::HTTPServerResponse &response, const Poco::JSON::Object &data);
    void BadRequest(Poco::Net::HTTPServerResponse &response);
    template <typename T> void Log(const std::string &key, const T &value);

    std::string m_name;
    std::shared_ptr<GeoIndexRegistry> m_registry;
    std::shared_ptr<Client> m_redisClient;
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
