#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <thread>
#include <mutex>
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

std::mutex mtx;   // mutex for critical section

const GeoIndex *pIndex;
std::vector<Object::Ptr> tempResults;
Array::Ptr targets;
Array::Ptr results;
double radius;
int count;
std::vector<std::string> pids;
std::vector<double> lats;
std::vector<double> lngs;
std::vector<std::ostringstream> threadLogs;

void batchProcessQuery(int id, int start, int end)
{
  float startTime = (float)clock()/CLOCKS_PER_SEC;
  for(int i=start; i<end; ++i)
  {
    Object::Ptr result(new Object);
    Array::Ptr points(new Array);
    
    auto pPoints = pIndex->QueryClosestPoints(GeoPoint(pids[i], lats[i], lngs[i]), count, radius);

    for (auto &point: *pPoints) {
      points->add((const Array::Ptr)point);
    }

    result->set("points", points);

    tempResults[i] = result;
    // mtx.lock();
    // results->add(result);
    // mtx.unlock();
  }

  float endTime = (float)clock()/CLOCKS_PER_SEC;
  threadLogs[id] << "Thread " << id << " processes queries[" << start << "," << end << "). It starts at " << startTime << ", end at " << endTime << ", duration " << endTime - startTime << " seconds.";
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
  results = new Array;

  m_performanceLogger.start("make-query");

  int num_query = targets->size();
  int num_threads = 16;
  num_threads = num_query > num_threads ? num_threads : num_query;
  threadLogs.clear();
  threadLogs.resize(num_threads);

  int batch_size = (num_query + num_threads - 1) / num_threads;

  tempResults = std::vector<Object::Ptr>(num_query);
  pids.resize(num_query);
  lats.resize(num_query);
  lngs.resize(num_query);

  int index = 0;
  m_performanceLogger.start("parsing-query");
  for (auto &i: *targets) {
    Array::Ptr targetPoint = i.extract<Array::Ptr>();

    pids[index] = targetPoint->get(0).toString();
    lats[index] = (double)targetPoint->get(1);
    lngs[index] = (double)targetPoint->get(2);
    ++index;
  }
  m_performanceLogger.finish("parsing-query");

  std::vector<std::thread> threads;

  m_performanceLogger.start("start-thread");
  for(int i=0; i<num_threads; ++i){
    int start = i * batch_size;
    int end = start + batch_size;
    if(end > num_query)
      end = num_query;
    threads.push_back(std::thread(batchProcessQuery, i, start, end));
  }
  m_performanceLogger.finish("start-thread");

  m_performanceLogger.start("run-thread");
  for (auto& thread : threads) {
      thread.join();
  }
  m_performanceLogger.finish("run-thread");

 for(int i=0; i<(int)tempResults.size(); ++i){
   results->add(tempResults[i]);
 }

  m_performanceLogger.finish("make-query");

  objRes->set("id", m_uuid);
  objRes->set("results", results);

  std::cout << "===================================== Processing KNN-" << count << "=====================================\n";
  std::cout <<"num_threads: " << num_threads << ", num_query: " << num_query << ", batch_size: " << batch_size << "\n";
  for (auto &pair: m_performanceLogger) {
    std::cout << pair.first << " takes " << pair.second << " seconds\n";
  }

  for(auto &ostr : threadLogs){
    std::cout << ostr.str() << "\n";
  }
  std::cout << "===================================== Finish of  KNN-" << count << "=====================================\n\n";

  SendResponse(response, objRes);
}

