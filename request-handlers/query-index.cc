#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <regex>
#include <thread>
#include "omp.h"

#include <Poco/JSON/Parser.h>
#include <Poco/URI.h>
// #include <Poco/Thread.h>
// #include <Poco/Runnable.h>

#include "query-index.h"

using std::string;
using std::vector;

using Poco::JSON::Object;
using Poco::JSON::Array;

const GeoIndex *pIndex;
std::vector<Object::Ptr> tempResults;
Array::Ptr targets;
double radius;
int count;

void batchProcessQuery(int start, int end)
{
  for(int i=start; i<end; ++i)
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
}

void QueryIndexRequestHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
  // Find the index
  Log("URI", request.getURI());

  pIndex = m_registry.GetGeoIndex(m_uuid);

  if (!pIndex) {
    BadRequest(response);
    return;
  }

  m_performanceLogger.start("parse-request-body");
  const Object::Ptr object = ReadRequestBody(request);
  m_performanceLogger.finish("parse-request-body");

  targets = object->get("points").extract<Array::Ptr>();
  count = object->get("count");
  radius = object->get("radius");

  Object::Ptr objRes(new Object);
  Array::Ptr results(new Array);

  m_performanceLogger.start("make-query");

  int num_query = targets->size();
  int num_threads = 16;
  num_threads = num_query > num_threads ? num_threads : num_query;
  
  int batch_size = (num_query + num_threads - 1) / num_threads;

  tempResults = std::vector<Object::Ptr>(num_query);

  std::cout <<"num_threads: " << num_threads << ", num_query: " << num_query << ", batch_size: " << batch_size << std::endl;

  std::vector<std::thread> threads;

  for(int i=0; i<num_threads; ++i){
    int start = i * batch_size;
    int end = start + batch_size;
    if(end > num_query)
      end = num_query;
    threads.push_back(std::thread(batchProcessQuery, start, end));
  }

  for (auto& thread : threads) {
      thread.join();
  }

  for(int i=0; i<(int)tempResults.size(); ++i){
    results->add(tempResults[i]);
  }

/*
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
*/
  m_performanceLogger.finish("make-query");

  objRes->set("id", m_uuid);
  objRes->set("results", results);

  SendResponse(response, objRes);
}

