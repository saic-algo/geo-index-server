#include <iostream>
#include <regex>

#include "geo-index-request-handler-factory.h"
#include "geo-index-request-handler.h"
#include "request-handlers/create-index.h"
#include "request-handlers/delete-index.h"
#include "request-handlers/query-index.h"
#include "request-handlers/create-index-redis.h"
#include "request-handlers/delete-index-redis.h"
#include "request-handlers/query-index-redis.h"

const std::regex GEO_INDEX_REGEX("^\\/GeoIndex(\\/.*)?$");
const std::regex GEO_INDEX_REDIS_REGEX("^\\/GeoIndexRedis(\\/.*)?$");

HTTPRequestHandler* GeoIndexRequestHandlerFactory::createRequestHandler(const HTTPServerRequest& request) {
  if (request.getURI() == "/") 
    return new GeoIndexRequestHandler();

  if (std::regex_match(request.getURI(), GEO_INDEX_REGEX)) {
    if (request.getMethod() == HTTPServerRequest::HTTP_GET)
      return new QueryIndexRequestHandler(m_registry, m_redisClient);

    if (request.getMethod() == HTTPServerRequest::HTTP_POST)
      return new CreateIndexRequestHandler(m_registry, m_redisClient);

    if (request.getMethod() == HTTPServerRequest::HTTP_DELETE) {
      return new DeleteIndexRequestHandler(m_registry, m_redisClient);
    }
  }
  else if (std::regex_match(request.getURI(), GEO_INDEX_REDIS_REGEX)) {
    if (request.getMethod() == HTTPServerRequest::HTTP_GET)
      return new RedisQueryIndexRequestHandler(m_registry, m_redisClient);

    if (request.getMethod() == HTTPServerRequest::HTTP_POST)
      return new RedisCreateIndexRequestHandler(m_registry, m_redisClient);

    if (request.getMethod() == HTTPServerRequest::HTTP_DELETE) {
      return new RedisDeleteIndexRequestHandler(m_registry, m_redisClient);
    }
  }


  return 0;
}

