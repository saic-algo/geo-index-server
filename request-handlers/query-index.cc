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

#include "query-index.h"

const std::regex UUID_REGEX("[a-f0-9]{8}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{12}");
const double DEFAULT_RADIUS = 100.00;

void QueryIndexRequestHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
  const Poco::URI uri(request.getURI());
  const std::string &path = uri.getPath();

  Log("URI", request.getURI());

  std::smatch matchUUID;

  if (std::regex_search(path, matchUUID, UUID_REGEX)) {
    // Find the index
    const auto pIndex = m_registry->find(matchUUID.str());
    if (pIndex == m_registry->end()) {
      BadRequest(response);
      return;
    }

    Log("IndexID", pIndex->first);

    // Process the query parameters
    const Poco::URI::QueryParameters query = uri.getQueryParameters();
    const std::unordered_map<std::string, std::string> params(query.begin(), query.end());

    const auto pLatitude = params.find("latitude");
    const auto pLongitude = params.find("longitude");
    const auto pCount = params.find("count");
    const auto pRadius = params.find("radius");

    if (pLatitude == params.end() || pLongitude == params.end() || pCount == params.end()) {
      response.setStatus(HTTPServerResponse::HTTP_BAD_REQUEST);
      return;
    }


    const double latitude = std::stod(pLatitude -> second);
    const double longitude = std::stod(pLongitude -> second);
    const int count = std::stoi(pCount -> second);
    const double radius = pRadius == params.end() ? DEFAULT_RADIUS : std::stod(pRadius->second) / 1000;

    Log("Latitude", latitude);
    Log("Longitude", longitude);
    Log("Count", count);
    Log("Radius", radius);

    // Make the query
    m_performanceLogger.start("query");
    const S2LatLng loc = S2LatLng::FromDegrees(latitude, longitude);
    S2ClosestPointQuery<std::string>::PointTarget target(loc.ToPoint());
    S2ClosestPointQuery<std::string> closestPointQuery(pIndex->second.get());

    closestPointQuery.mutable_options()->set_max_points(count);
    closestPointQuery.mutable_options()->set_max_distance(S1Angle::Radians(S2Earth::KmToRadians(radius)));

    auto queryResult = closestPointQuery.FindClosestPoints(&target);
    m_performanceLogger.finish("query");

    const int size = queryResult.size();

    Log("ResultSize", size);

    response.setContentType("text/json");

    // Generate the output
    Poco::JSON::Object result;
    result.set("id", matchUUID.str());
    result.set("count", queryResult.size());

    Poco::JSON::Array points;
    for (const S2ClosestPointQuery<std::string>::Result &result: queryResult) {
      Poco::JSON::Object point;
      S2LatLng loc(result.point());

      point.set("id", result.data());
      point.set("latitude", loc.lat().degrees());
      point.set("longitude", loc.lng().degrees());
      points.add(point);
    }
    result.set("points",  points);
    SendResponse(response, result);
  }
}

