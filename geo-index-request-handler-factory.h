#ifndef __GEO_INDEX_REQUEST_HANDLER_FACTORY__
#define __GEO_INDEX_REQUEST_HANDLER_FACTORY__

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>


using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServerRequest;

class GeoIndexRequestHandlerFactory : public HTTPRequestHandlerFactory {
  public:
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request);
};


#endif // __GEO_INDEX_REQUEST_HANDLER__
