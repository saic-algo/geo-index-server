#include <iostream>
#include <regex>

#include <Poco/JSON/Object.h>

#include "delete-index-redis.h"

const std::regex UUID_REGEX("[a-f0-9]{8}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{12}");

void RedisDeleteIndexRequestHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
  const std::string &uri = request.getURI();
  std::smatch matchUUID;

#ifdef DEBUG 
  std::cout << "Delete Index" << std::endl;
  std::cout << "URI: " << uri << std::endl;
#endif // DEBUG

  if (std::regex_search(uri, matchUUID, UUID_REGEX)) {
    std::ostream &ostm = response.send();
    Poco::JSON::Object result;

    result.set("id", matchUUID.str());

    m_registry->erase(matchUUID.str());
    result.stringify(ostm);
  }
}

