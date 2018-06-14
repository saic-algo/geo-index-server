#include <iostream>
#include <regex>

#include "geo-index-request-handler-factory.h"
#include "geo-index-request-handler.h"
#include "s2-geo-index.h"

#include "request-handlers/create-index.h"
#include "request-handlers/delete-index.h"
#include "request-handlers/query-index.h"
#include"request-handlers/query-latest-index.h"
#include"request-handlers/delete-latest-index.h"

#define H(n) "[0-9a-f]{" #n "}"
#define REGEX_UUID H(8) "-" H(4) "-" H(4) "-" H(4) "-" H(12)

const std::regex REGEX_CREATE_GEO_INDEX("^\\/GeoIndex\\/?$");
const std::regex REGEX_GEO_INDEX_V2("^\\/GeoIndex\\/latest/?$");
const std::regex REGEX_GEO_INDEX("^\\/GeoIndex\\/(" REGEX_UUID ")$");


HTTPRequestHandler* GeoIndexRequestHandlerFactory::createRequestHandler(const HTTPServerRequest& request) {
  const std::string &uri = request.getURI();
  const std::string &method = request.getMethod();
	//std::cout<<uri<<std::endl;

  if (std::regex_match(uri, REGEX_CREATE_GEO_INDEX) && method == HTTPServerRequest::HTTP_POST) {
    return new CreateIndexRequestHandler(m_registry);
  }

  std::smatch match;

  if (std::regex_match(uri, match, REGEX_GEO_INDEX)) {
    if (method == HTTPServerRequest::HTTP_POST)
      return new QueryIndexRequestHandler(m_registry, match[1]);

    if (method == HTTPServerRequest::HTTP_DELETE)
      std::cout << "Get here" << std::endl;
      return new DeleteIndexRequestHandler(m_registry, match[1]);
  }

	if (std::regex_match(uri, REGEX_GEO_INDEX_V2)) {
		if (method == HTTPServerRequest::HTTP_POST) return new QueryLatestIndex(m_registry);
		if (method == HTTPServerRequest::HTTP_DELETE) return new DeleteLatestIndex(m_registry);
	}
	std::cout<<"match_fail uri="<<uri<<std::endl;
  return 0;
}

