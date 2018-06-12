#include <memory>

#include "create-index.h"
#include "../s2-geo-index.h"
#include "../redis-geo-index.h"
#include "../h3-geo-index.h"

using std::string;
using std::unique_ptr;
using std::make_unique;
using std::move;

using Poco::JSON::Object;
using Poco::JSON::Array;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

const char *REDIS_HOST = "127.0.0.1";
const unsigned short REDIS_PORT = 6379;

void CreateIndexRequestHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
  Log("URI", request.getURI());

  const Object::Ptr object = ReadRequestBody(request);

  m_performanceLogger.start("build-index");
  Array::Ptr points = object->get("points").extract<Array::Ptr>();
  string type = object->get("type");

  Log("Type", type);

  unique_ptr<GeoIndex> index;
  
  if (type == "redis") {
    // Redis
    index = make_unique<RedisGeoIndex>(REDIS_HOST, REDIS_PORT);
  } else if (type == "h3") {
    index = make_unique<H3GeoIndex>();
  } else {
    // S2
    index = make_unique<S2GeoIndex>();
  }

  for (auto &i: *points) {
    Array::Ptr point = i.extract<Array::Ptr>();
    const string &id = point->get(0).toString();
    const double lat = (double)point->get(1);
    const double lng = (double)point->get(2);

    index->AddPoint(GeoPoint(id, lat, lng));
  }
  m_performanceLogger.finish("build-index");

  Object::Ptr result(new Object);
  result->set("id", index->UUID());

  m_registry.AddGeoIndex(move(index));
  SendResponse(response, result);
}
