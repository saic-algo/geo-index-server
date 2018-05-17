#include <iostream>
#include <memory>
#include <ctime>

#include <Poco/UUIDGenerator.h>
#include <Poco/JSON/Parser.h>


#include <s2/s2latlng.h>

#include "create-index.h"

using std::istream;
using std::ostream;
using std::endl;
using std::make_unique;

using Poco::UUIDGenerator;
using Poco::JSON::Parser;
using Poco::JSON::Object;
using Poco::JSON::Array;
using Poco::Dynamic::Var;

std::unique_ptr<std::string> CreateIndexRequestHandler::ReadRequestBody(HTTPServerRequest &request) {
  istream &istm = request.stream();
  size_t length = request.getContentLength();
  auto buffer = make_unique<char[]>(length);
  istm.read(buffer.get(), length);

  return std::make_unique<std::string>(buffer.get(), length);
}

std::unique_ptr<GeoIndex> CreateIndexRequestHandler::CreateIndex(const std::string &input) {
  m_performanceLogger.start("parse-request-body");
  Poco::JSON::Parser parser;
  Object::Ptr object = parser.parse(input).extract<Object::Ptr>();
  m_performanceLogger.finish("parse-request-body");

  m_performanceLogger.start("build-index");
  std::unique_ptr<GeoIndex> index = std::make_unique<GeoIndex>();
  Array::Ptr logs = object->get("points").extract<Array::Ptr>();

  for (auto &i: *logs) {
    Array::Ptr log = i.extract<Array::Ptr>();
    const std::string &id = log->get(0).toString();
    const double lat = (double)log->get(1);
    const double lng = (double)log->get(2);
    S2LatLng loc = S2LatLng::FromDegrees(lat, lng);

    index->Add(loc.ToPoint(), id);
  }
  m_performanceLogger.finish("build-index");

  return std::move(index);
}

void CreateIndexRequestHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
  Poco::UUID uuid = UUIDGenerator::defaultGenerator().create();
  std::unique_ptr<std::string> body = ReadRequestBody(request);
  std::unique_ptr<GeoIndex> index = CreateIndex(*body);

  m_registry->insert(std::make_pair(uuid.toString(), std::move(index)));

  response.setChunkedTransferEncoding(true);
  response.setContentType("text");
  ostream &ostm = response.send();

  Poco::JSON::Object result;
  result.set("id", uuid.toString());
  for (auto &pair: m_performanceLogger) {
    result.set(pair.first, pair.second);
  }
  result.stringify(ostm);
}

