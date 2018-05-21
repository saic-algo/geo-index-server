#include <iostream>

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "base.h"

using std::cout;
using std::endl;
using std::string;
using std::istream;
using std::ostream;
using std::make_unique;

using Poco::JSON::Object;
using Poco::JSON::Array;
using Poco::JSON::Parser;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

BaseRequestHandler::BaseRequestHandler(const char *pName, GeoIndexRegistry &registry)
  : m_name(pName), m_registry(registry)
{
#ifdef DEBUG
  std::cout << "==== " << m_name << " Begin ===" << std::endl;
#endif // DEBUG
}

BaseRequestHandler::~BaseRequestHandler() {
#ifdef DEBUG
  std::cout << "==== " << m_name << " End ===" << std::endl;
  std::cout << std::endl;
#endif // DEBUG
}


template <>
void BaseRequestHandler::Log(const string &key, const Object::Ptr &value) {
#ifdef DEBUG
  cout << "[" << m_name << "/" << key << "]: " << std::endl;
  value->stringify(cout, 2);
  cout << endl;
#endif // DEBUG
}

template <>
void BaseRequestHandler::Log(const string &key, const Array::Ptr &value) {
#ifdef DEBUG
  cout << "[" << m_name << "/" << key << "]: " << std::endl;
  value->stringify(cout, 2);
  cout << endl;
#endif // DEBUG
}

const Object::Ptr BaseRequestHandler::ReadRequestBody(HTTPServerRequest &request) {
  m_performanceLogger.start("parse-request-body");
  const Object::Ptr object = Parser().parse(request.stream()).extract<Object::Ptr>();
  m_performanceLogger.finish("parse-request-body");

  Log("RequestBody", object);

  return object;
}

void BaseRequestHandler::BadRequest(HTTPServerResponse &response) {
  response.setStatusAndReason(HTTPServerResponse::HTTP_BAD_REQUEST, HTTPServerResponse::HTTP_REASON_BAD_REQUEST);
}

void BaseRequestHandler::SendResponse(HTTPServerResponse &response, const Object::Ptr data) {
  Object::Ptr dataCopy(new Object(*data));
  Object::Ptr performanceLogs(new Object);

  for (auto &pair: m_performanceLogger) {
    performanceLogs->set(pair.first, pair.second);
  }

  dataCopy->set("@performance-logs", performanceLogs);

  response.setChunkedTransferEncoding(true);
  response.setContentType("text");

  ostream &ostm = response.send();

  Log("Response", dataCopy);

  dataCopy->stringify(ostm);
}

