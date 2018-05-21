#include <memory>

#include "create-index.h"

using std::string;
using std::unique_ptr;
using std::move;

using Poco::JSON::Object;
using Poco::JSON::Array;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

void CreateIndexRequestHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
  Log("URI", request.getURI());

  const Object::Ptr object = ReadRequestBody(request);

  m_performanceLogger.start("build-index");
  unique_ptr<GeoIndex> index = m_factory.CreateGeoIndex();
  Array::Ptr points = object->get("points").extract<Array::Ptr>();

  for (auto &i: *points) {
    Array::Ptr point = i.extract<Array::Ptr>();
    const string &id = point->get(0).toString();
    const double lat = (double)point->get(1);
    const double lng = (double)point->get(2);

    index->AddPoint(GeoPoint(id, lat, lng));
  }
  m_performanceLogger.finish("build-index");

  Object::Ptr result(new Object);
  string uuid = m_registry.AddGeoIndex(move(index));
  result->set("id", uuid);
  SendResponse(response, result);
}

