#include <iostream>

#include <Poco/JSON/Object.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "base.h"

using std::cout;
using std::endl;
using std::string;
using std::ostream;
using Poco::Net::HTTPServerResponse;
using Poco::JSON::Object;

BaseRequestHandler::BaseRequestHandler(const char *pName, std::shared_ptr<GeoIndexRegistry> registry)
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
void BaseRequestHandler::Log(const string &key, const Object &value) {
#ifdef DEBUG
  cout << "[" << m_name << "/" << key << "]: ";
  value.stringify(cout);
  cout << endl;
#endif // DEBUG
}

void BaseRequestHandler::BadRequest(HTTPServerResponse &response) {
  response.setStatusAndReason(HTTPServerResponse::HTTP_BAD_REQUEST, HTTPServerResponse::HTTP_REASON_BAD_REQUEST);
}

void BaseRequestHandler::SendResponse(HTTPServerResponse &response, const Object &data) {
  Object dataCopy(data);
  Object performanceLogs;

  for (auto &pair: m_performanceLogger) {
    performanceLogs.set(pair.first, pair.second);
  }

  dataCopy.set("@performance-logs", performanceLogs);

  response.setChunkedTransferEncoding(true);
  response.setContentType("text");

  ostream &ostm = response.send();

  Log("Response", dataCopy);

  dataCopy.stringify(ostm);
}
