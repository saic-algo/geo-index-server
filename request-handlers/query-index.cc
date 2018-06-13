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
  std::cout<<request.getURI()<<std::endl;
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
  std::vector<Object> ret(targets->size());

  m_performanceLogger.start("make-query");

  int num_threads = 16;
  omp_set_num_threads(num_threads);
  std::cout << "Max num threads: " << omp_get_max_threads() << ", Num threads: " << omp_get_num_threads() << ", thread id " << omp_get_thread_num() << std::endl;
  double starttime=omp_get_wtime();

#pragma omp parallel for num_threads(num_threads)
  for(int i=0; i<(int)targets->size(); ++i)
  {
    // std::cout << "Max num threads: " << omp_get_max_threads() << ", Num threads: " << omp_get_num_threads() << ", thread id " << omp_get_thread_num() << std::endl;
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

    //result->set("points", points);
    ret[i].set("points",points);
/*
#pragma omp critical
    {
      results->add(result);
    }
*/
  }
  for(int i=0;i<(int)targets->size();++i) results->add(ret[i]);
  double endtime=omp_get_wtime();
  std::cout << "Using " << num_threads << " threads. Query starts at " << starttime << ", finishes at " << endtime << ", duration " << endtime - starttime << " seconds."<<std::endl;

  m_performanceLogger.finish("make-query");

  objRes->set("id", m_uuid);
  objRes->set("results", results);

  SendResponse(response, objRes);
}

