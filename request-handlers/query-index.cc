#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <regex>
#include "omp.h"

#include <Poco/JSON/Parser.h>
#include <Poco/URI.h>
#include <Poco/Thread.h>
#include <Poco/Runnable.h>

#include "query-index.h"

using std::string;
using std::vector;

using Poco::JSON::Object;
using Poco::JSON::Array;

class QueryRunnable : public Poco::Runnable
{
public:
  static int id;
  QueryRunnable(const GeoIndex *pIndex, Array::Ptr targets, int count, double radius, int start, int end, vector<Object::Ptr>& results)
  : pIndex(pIndex), targets(targets), count(count), radius(radius), start(start), end(end), results(results) { 
    id = id+1;
    std::cout << "Create QueryRunnable " << id << std::endl;
  }

  virtual void run()
  {
    std::cout << "here0(" << start << "," <<end << ")" << std::endl;
    for(int i=start; i<end; ++i)
    {
      Object::Ptr result(new Object);
      Array::Ptr points(new Array);

      auto gg = targets->get(i);
      Array::Ptr targetPoint = gg.extract<Array::Ptr>();

      const string &id = targetPoint->get(0).toString();
      const double lat = (double)targetPoint->get(1);
      const double lng = (double)targetPoint->get(2);
      
      std::cout << "here1" << std::endl;
      auto pPoints = pIndex->QueryClosestPoints(GeoPoint(id, lat, lng), count, radius);

      for (auto &point: *pPoints) {
        points->add((const Array::Ptr)point);
      }

      result->set("points", points);

      results[i] = result;
    }
    std::cout << "here2" << std::endl;
  }

private:
  const GeoIndex *pIndex;
  const Array::Ptr targets;
  int start, end, count;
  double radius;
  vector<Object::Ptr>& results;
};

int QueryRunnable::id = 0;

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

  int num_threads = 10;
  int num_query = targets->size();
  int batch_size = (num_query + num_threads - 1) / num_threads;
  std::cout <<"num_threads: " << num_threads << ", num_query: " << num_query << ", batch_size: " << batch_size << std::endl;

  std::vector<Poco::Thread> threads(num_threads);

  for(int i=0; i<num_threads; ++i){
    int start = i * batch_size;
    int end = start + batch_size;
    if(end > num_query)
      end = num_query;
      QueryRunnable hello(pIndex, targets, count, radius, start, end, tempResults);
      threads[i].start(hello);
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

