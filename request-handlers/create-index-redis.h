#ifndef __REQUEST_HANDLER__CREATE_INDEX__
#define __REQUEST_HANDLER__CREATE_INDEX__

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "base.h"

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

class RedisCreateIndexRequestHandler : public BaseRequestHandler {
  public:
    RedisCreateIndexRequestHandler(std::shared_ptr<GeoIndexRegistry> registry)
      : BaseRequestHandler(registry) { }
  public:
    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response);
  private:
    std::unique_ptr<std::string> ReadRequestBody(HTTPServerRequest &request);
    std::unique_ptr<GeoIndex> CreateIndex(const std::string &input);
};

#endif // __REQUEST_HANDLER__CREATE_INDEX__

