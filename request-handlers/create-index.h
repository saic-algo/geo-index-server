#ifndef __REQUEST_HANDLER__CREATE_INDEX__
#define __REQUEST_HANDLER__CREATE_INDEX__

#include <memory>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "../geo-index.h"

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

class CreateIndexRequestHandler : public HTTPRequestHandler {
  public:
    CreateIndexRequestHandler(std::shared_ptr<GeoIndexRegistry> registry) : m_registry(registry) { };

  public:
    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response);

  private:
    std::shared_ptr<GeoIndexRegistry> m_registry;
};

#endif // __REQUEST_HANDLER__CREATE_INDEX__

