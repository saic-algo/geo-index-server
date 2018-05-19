#ifndef __REQUEST_HANDLER__QUERY_INDEX__
#define __REQUEST_HANDLER__QUERY_INDEX__

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "base.h"

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

class QueryIndexRequestHandler : public BaseRequestHandler {
  public:
    QueryIndexRequestHandler(std::shared_ptr<GeoIndexRegistry> registry, std::shared_ptr<Client> client)
      : BaseRequestHandler(registry, client) { }
  public:
    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response);
};

#endif // __REQUEST_HANDLER__QUERY_INDEX__

