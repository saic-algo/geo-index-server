#ifndef __REQUEST_HANDLER__DELETE_INDEX__
#define __REQUEST_HANDLER__DELETE_INDEX__

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "base.h"

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

class DeleteIndexRequestHandler : public BaseRequestHandler {
  public:
    DeleteIndexRequestHandler(std::shared_ptr<GeoIndexRegistry> registry, std::shared_ptr<Client> client)
      : BaseRequestHandler("DeleteIndex", registry, client) { }
  public:
    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response);
};

#endif // __REQUEST_HANDLER__DELETE_INDEX__

