#ifndef __REQUEST_HANDLER__DELETE_INDEX_REDIS__
#define __REQUEST_HANDLER__DELETE_INDEX_REDIS__

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "base.h"

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

class RedisDeleteIndexRequestHandler : public BaseRequestHandler {
  public:
    RedisDeleteIndexRequestHandler(std::shared_ptr<GeoIndexRegistry> registry, std::shared_ptr<Client> client)
      : BaseRequestHandler("DeleteIndexRedis", registry, client) { }
  public:
    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response);
};

#endif // __REQUEST_HANDLER__DELETE_INDEX_REDIS__

