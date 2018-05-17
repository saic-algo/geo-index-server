#include "geo-index-request-handler-factory.h"
#include "geo-index-request-handler.h"

HTTPRequestHandler* GeoIndexRequestHandlerFactory::createRequestHandler(const HTTPServerRequest& request) {
  if (request.getURI() == "/") 
    return new GeoIndexRequestHandler();
  else
    return 0;
}



