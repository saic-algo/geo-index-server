#include <iostream>

#include "geo-index-request-handler-factory.h"
#include "geo-index-request-handler.h"
#include "request-handlers/create-index.h"

HTTPRequestHandler* GeoIndexRequestHandlerFactory::createRequestHandler(const HTTPServerRequest& request) {
  if (request.getURI() == "/") 
    return new GeoIndexRequestHandler();
  if (request.getURI() == "/create-index" && request.getMethod() == HTTPServerRequest::HTTP_POST)
    return new CreateIndexRequestHandler(m_registry);
  else
    return 0;
}

