#ifndef __REQUEST_HANDLER__CREATE_INDEX_REDIS__
#define __REQUEST_HANDLER__CREATE_INDEX_REDIS__

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "base.h"

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

class RedisCreateIndexRequestHandler : public BaseRequestHandler {
  public:
    RedisCreateIndexRequestHandler(std::shared_ptr<GeoIndexRegistry> registry, std::shared_ptr<Client> client)
      : BaseRequestHandler(registry, client) { }
  public:
    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response);
  private:
    std::unique_ptr<std::string> ReadRequestBody(HTTPServerRequest &request);
    void CreateIndex(const std::string &input, std::string &indexId);
};

#endif // __REQUEST_HANDLER__CREATE_INDEX_REDIS__

