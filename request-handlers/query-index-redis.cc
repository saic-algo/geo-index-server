#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <regex>

#include <Poco/JSON/Parser.h>
#include <Poco/URI.h>

#include <s2/s1angle.h>
#include <s2/s2latlng.h>
#include <s2/s2earth.h>
#include <s2/s2closest_point_query.h>

#include "query-index-redis.h"

const std::regex UUID_REGEX("[a-f0-9]{8}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{12}");
const double DEFAULT_RADIUS = 100.00;

void RedisQueryIndexRequestHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
  const Poco::URI uri(request.getURI());
  const std::string &path = uri.getPath();

#ifdef DEBUG
  std::cout << "Query index" << std::endl;
  std::cout << "URI: " << request.getURI() << std::endl;
#endif // DEBUG

  std::smatch matchUUID;
  std::ostream &ostm = response.send();

  if (std::regex_search(path, matchUUID, UUID_REGEX)) {
    // Find the index
    const auto pIndex = m_registry->find(matchUUID.str());
    if (pIndex == m_registry->end()) {
      response.setStatus(HTTPServerResponse::HTTP_BAD_REQUEST);
      ostm << HTTPServerResponse::HTTP_REASON_BAD_REQUEST;
      return;
    }
#ifdef DEBUG
    std::cout << "Index ID: " << pIndex->first << std::endl;
#endif // DEBUG

    // Process the query parameters
    const Poco::URI::QueryParameters query = uri.getQueryParameters();
    const std::unordered_map<std::string, std::string> params(query.begin(), query.end());

    const auto pLatitude = params.find("latitude");
    const auto pLongitude = params.find("longitude");
    const auto pCount = params.find("count");
    const auto pRadius = params.find("radius");

    if (pLatitude == params.end() || pLongitude == params.end() || pCount == params.end()) {
      response.setStatus(HTTPServerResponse::HTTP_BAD_REQUEST);
      ostm << HTTPServerResponse::HTTP_REASON_BAD_REQUEST;
      return;
    }


    const double latitude = std::stod(pLatitude -> second);
    const double longitude = std::stod(pLongitude -> second);
    const int count = std::stoi(pCount -> second);
    const double radius = pRadius == params.end() ? DEFAULT_RADIUS : std::stod(pRadius->second) / 1000;

#ifdef DEBUG
    std::cout << "Latitude: " << latitude << std::endl;
    std::cout << "Longitude: " << longitude << std::endl;
    std::cout << "Count: " << count << std::endl;
    std::cout << "Radius: " << radius << std::endl;
#endif // DEBUG

    // Build the query
    const S2LatLng loc = S2LatLng::FromDegrees(latitude, longitude);
    S2ClosestPointQuery<std::string>::PointTarget target(loc.ToPoint());
    S2ClosestPointQuery<std::string> closestPointQuery(pIndex->second.get());

    closestPointQuery.mutable_options()->set_max_points(count);
    closestPointQuery.mutable_options()->set_max_distance(S1Angle::Radians(S2Earth::KmToRadians(radius)));

    auto queryResult = closestPointQuery.FindClosestPoints(&target);

    const int size = queryResult.size();
#ifdef DEBUG
    std::cout << "Result Size: " << size << std::endl;
#endif // DEBUG

    response.setContentType("text/json");

    // Generate the output
    Poco::JSON::Object result;
    result.set("id", matchUUID.str());
    result.set("count", queryResult.size());
    for (auto &pair: m_performanceLogger) {
      result.set(pair.first, pair.second);
    }

#ifdef DEBUG
    result.stringify(std::cout);
#endif // DEBUG
    result.stringify(ostm);
  }
}

