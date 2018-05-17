#ifndef __GEO_INDEX_REQUEST_HANDLER__
#define __GEO_INDEX_REQUEST_HANDLER__

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

class GeoIndexRequestHandler : public HTTPRequestHandler {
  public:
    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response);
};

#endif // __GEO_INDEX_REQUEST_HANDLER__

