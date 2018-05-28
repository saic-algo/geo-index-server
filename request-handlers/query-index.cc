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

  vector<Object::Ptr> tempResults(targets->size());

  m_performanceLogger.start("make-query");

  #pragma omp parallel for
  for (int i=0; i<(int)targets->size(); ++i){
    Object::Ptr result(new Object);
    Array::Ptr points(new Array);

    auto gg = targets->get(i);
    Array::Ptr targetPoint = gg.extract<Array::Ptr>();

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

  for (auto &i: *targets) {
    Object::Ptr result(new Object);
    Array::Ptr points(new Array);

    Array::Ptr targetPoint = i.extract<Array::Ptr>();
    result->set("target", targetPoint);

    const string &id = targetPoint->get(0).toString();
    const double lat = (double)targetPoint->get(1);
    const double lng = (double)targetPoint->get(2);

    auto pPoints = pIndex->QueryClosestPoints(GeoPoint(id, lat, lng), count, radius);

    for (auto &point: *pPoints) {
      points->add((const Array::Ptr)point);
    }

    result->set("points", points);

    results->add(result);
  }
  m_performanceLogger.finish("make-query");

  objRes->set("id", m_uuid);
  objRes->set("results", results);

  SendResponse(response, objRes);
}

