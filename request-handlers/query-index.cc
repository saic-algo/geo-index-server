#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <regex>
#include "omp.h"

#include <Poco/JSON/Parser.h>
#include <Poco/URI.h>

#include "query-index.h"

using std::string;
using std::vector;

using Poco::JSON::Object;
using Poco::JSON::Array;

const int num_threads = 8;

void QueryIndexRequestHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
  // Find the index
  Log("URI", request.getURI());

  const GeoIndex *pIndex = m_registry.GetGeoIndex(m_uuid);

  if (!pIndex) {
    BadRequest(response);
    return;
  }

  m_performanceLogger.start("parse-request-body");
  const Object::Ptr object = ReadRequestBody(request);
  m_performanceLogger.finish("parse-request-body");

  const Array::Ptr targets = object->get("points").extract<Array::Ptr>();
  const int count = object->get("count");
  const double radius = object->get("radius");

  Object::Ptr objRes(new Object);
  Array::Ptr results(new Array);

  m_performanceLogger.start("make-query");
  std::vector<Object::Ptr> tempResults(targets->size());

#pragma omp parallel for num_threads(num_threads)
  for(int i=0; i<(int)targets->size(); ++i)
  {
    Object::Ptr result(new Object);
    Array::Ptr points(new Array);

    Array::Ptr targetPoint = targets->get(i).extract<Array::Ptr>();

    const string &id = targetPoint->get(0).toString();
    const double lat = (double)targetPoint->get(1);
    const double lng = (double)targetPoint->get(2);

    auto pPoints = pIndex->QueryClosestPoints(GeoPoint(id, lat, lng), count, radius);

    for (auto &point: *pPoints) {
      points->add((const Array::Ptr)point);
    }

    result->set("points", points);

    tempResults[i] = result;
  }

  for(auto &result : tempResults){
    results->add(result);
  }

  m_performanceLogger.finish("make-query");

  objRes->set("id", m_uuid);
  objRes->set("results", results);

  SendResponse(response, objRes);
}
