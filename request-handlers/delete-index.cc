#include <Poco/JSON/Object.h>

#include "delete-index.h"

using std::string;

using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::JSON::Object;

void DeleteIndexRequestHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
  Log("URI", request.getURI());

  m_registry.DelGeoIndex(m_uuid);

  Object::Ptr result(new Object);
  result->set("id", m_uuid);
  SendResponse(response, result);
}

