#include <iostream>
#include <memory>

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

std::unique_ptr<std::string> ReadRequestBody(HTTPServerRequest &request) {
  istream &istm = request.stream();
  size_t length = request.getContentLength();
  auto buffer = make_unique<char[]>(length);
  istm.read(buffer.get(), length);

  return std::make_unique<std::string>(buffer.get(), length);
}

std::unique_ptr<GeoIndex> CreateIndex(const std::string &input) {
  std::unique_ptr<GeoIndex> index = std::make_unique<GeoIndex>();


  Poco::JSON::Parser parser;
  Object::Ptr object = parser.parse(input).extract<Object::Ptr>();
  Array::Ptr logs = object->get("points").extract<Array::Ptr>();

  for (auto &i: *logs) {
    Array::Ptr log = i.extract<Array::Ptr>();
    const std::string &id = log->get(0).toString();
    const double lat = (double)log->get(1);
    const double lng = (double)log->get(2);
    S2LatLng loc = S2LatLng::FromDegrees(lat, lng);

    index->Add(loc.ToPoint(), id);
  }

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
  ostm << uuid.toString() << endl;

  /*

    Parser parser;
    Var result = parser.parse(*body);
    Object::Ptr object = result.extract<Object::Ptr>();

    std::cout << (int)object->get("Hello") << std::endl;

    Array::Ptr array = object->get("Array").extract<Array::Ptr>();

    for (auto &i: *array) {
      std::cout << (int)i << std::endl;
    }


    std::cout << uuid.toString() << std::endl;


    ostream &ostr = response.send();
    ostr << *body << endl;
  */
}

