#ifndef __REQUEST_HANDLER__CREATE_INDEX__
#define __REQUEST_HANDLER__CREATE_INDEX__

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "base.h"

class CreateIndexRequestHandler : public BaseRequestHandler {
  public:
    CreateIndexRequestHandler(GeoIndexRegistry &registry)
      : BaseRequestHandler("CreateIndex", registry) { }

  public:
    void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
};

#endif // __REQUEST_HANDLER__CREATE_INDEX__

