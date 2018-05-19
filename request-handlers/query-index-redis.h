#ifndef __REQUEST_HANDLER__QUERY_INDEX_REDIS__
#define __REQUEST_HANDLER__QUERY_INDEX_REDIS__

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "base.h"

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Redis::Client;

class RedisQueryIndexRequestHandler : public BaseRequestHandler {
  public:
    RedisQueryIndexRequestHandler(std::shared_ptr<GeoIndexRegistry> registry, std::shared_ptr<Client> client)
      : BaseRequestHandler(registry, client) { }
  public:
    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response);
};

#endif // __REQUEST_HANDLER__QUERY_INDEX_REDIS__

