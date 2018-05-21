#include <iostream>
#include <memory>
#include <ctime>

#include <Poco/UUIDGenerator.h>
#include <Poco/JSON/Parser.h>

#include <s2/s2latlng.h>

#include "create-index-redis.h"

using std::istream;
using std::ostream;
using std::endl;
using std::make_unique;

using Poco::UUIDGenerator;
using Poco::JSON::Parser;
using Poco::JSON::Object;
using Poco::JSON::Array;
using Poco::Dynamic::Var;

std::unique_ptr<std::string> RedisCreateIndexRequestHandler::ReadRequestBody(HTTPServerRequest &request) {
  istream &istm = request.stream();
  size_t length = request.getContentLength();
  auto buffer = make_unique<char[]>(length);
  istm.read(buffer.get(), length);

  return std::make_unique<std::string>(buffer.get(), length);
}

int RedisCreateIndexRequestHandler::CreateIndex(const std::string &input, const std::string &indexId) {
  int ret = 0;

  m_performanceLogger.start("parse-request-body");
  Poco::JSON::Parser parser;
  Object::Ptr object = parser.parse(input).extract<Object::Ptr>();
  m_performanceLogger.finish("parse-request-body");

  m_performanceLogger.start("build-index");
  Array::Ptr logs = object->get("points").extract<Array::Ptr>();

  for (auto &i: *logs) {
    Array::Ptr log = i.extract<Array::Ptr>();
    const std::string &id = log->get(0).toString();
    const std::string &lat = log->get(1).toString();
    const std::string &lng = log->get(2).toString();

    Poco::Redis::Array cmd;
    cmd << "geoadd" << indexId << lng << lat << id;
    auto res = m_redisClient->execute<Poco::Int64>(cmd);
    ret += res;
  }
  m_performanceLogger.finish("build-index");

  return ret;
}

void RedisCreateIndexRequestHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
  Poco::UUID uuid = UUIDGenerator::defaultGenerator().create();
  std::unique_ptr<std::string> body = ReadRequestBody(request);
  auto indexId = uuid.toString();  
  int nPoints = CreateIndex(*body, indexId);  

  Log("URI", request.getURI());
  Log("Create Index Total Points", nPoints);
  Poco::JSON::Object result;
  result.set("id", indexId);

  SendResponse(response, result);
}

