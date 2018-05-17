#ifndef __REQUEST_HANDLER__CREATE_INDEX__
#define __REQUEST_HANDLER__CREATE_INDEX__

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

class CreateIndexRequestHandler : public HTTPRequestHandler {
  public:
    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response);
};

#endif // __REQUEST_HANDLER__CREATE_INDEX__

