#include <iostream>
#include <regex>

#include "geo-index-request-handler-factory.h"
#include "geo-index-request-handler.h"
#include "request-handlers/create-index.h"
#include "request-handlers/delete-index.h"

const std::regex GEO_INDEX_REGEX("^\\/GeoIndex(\\/.*)?$");

HTTPRequestHandler* GeoIndexRequestHandlerFactory::createRequestHandler(const HTTPServerRequest& request) {
  if (request.getURI() == "/") 
    return new GeoIndexRequestHandler();

  if (std::regex_match(request.getURI(), GEO_INDEX_REGEX)) {
    if (request.getMethod() == HTTPServerRequest::HTTP_POST)
      return new CreateIndexRequestHandler(m_registry);

    if (request.getMethod() == HTTPServerRequest::HTTP_DELETE) {
      return new DeleteIndexRequestHandler(m_registry);
    }
  }

  return 0;
}

