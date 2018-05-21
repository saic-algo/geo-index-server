#include <iostream>
#include <memory>
#include <vector>
#include <utility>
#include <string>
#include <regex>

#include <Poco/JSON/Parser.h>
#include <Poco/URI.h>
#include <Poco/Redis/Type.h>

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

  Log("URI", request.getURI());

  std::smatch matchUUID;

  if (std::regex_search(path, matchUUID, UUID_REGEX)) {

    // Process the query parameters
    const Poco::URI::QueryParameters query = uri.getQueryParameters();
    const std::unordered_map<std::string, std::string> params(query.begin(), query.end());

    const auto pLatitude = params.find("latitude");
    const auto pLongitude = params.find("longitude");
    const auto pCount = params.find("count");
    const auto pRadius = params.find("radius");

    if (pLatitude == params.end() || pLongitude == params.end() || pCount == params.end()) {
      BadRequest(response);
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

    // Build the query
    m_performanceLogger.start("query");
    auto queryResult = knnQuery(matchUUID.str(), std::to_string(longitude), std::to_string(latitude), count , radius);
    m_performanceLogger.finish("query");
    
    const int size = queryResult.size();

    response.setContentType("text/json");

    Log("ResultSize", size);

    // Generate the output
    Poco::JSON::Object result;
    result.set("id", matchUUID.str());
    result.set("count", queryResult.size());
    for (auto &pair: m_performanceLogger) {
      result.set(pair.first, pair.second);
    }

    Poco::JSON::Array points;
    for(const GeoPoint& p : queryResult) {
      Poco::JSON::Object point;
      point.set("id", p.first);
      point.set("latitude", p.second.second);
      point.set("longitude", p.second.first);
      points.add(point);
    }
    result.set("points", points);

    SendResponse(response, result);
  }
}

std::vector<GeoPoint> RedisQueryIndexRequestHandler::knnQuery(const std::string &indexId, const std::string &lng, const std::string &lat, int n, const double &maxRadius)
{
  double currectRadius = 0.1;
  Poco::Redis::Array result;
  do {
    Poco::Redis::Array cmd;
    cmd << "georadius" << indexId << lng << lat << std::to_string(currectRadius) << "km" << "ASC" << "WITHCOORD";
    result = m_redisClient->execute<Poco::Redis::Array>(cmd);
    currectRadius = currectRadius * 2;
  } while(result.size() < n && currectRadius < maxRadius);
 
  std::vector<GeoPoint> ret(n);
  GeoPoint point;
  for(int i=0; i<n && i<(int)result.size(); ++i){
    auto item = result.get<Poco::Redis::Array>(i);
    auto carId = item.get<Poco::Redis::BulkString>(0).value();
    auto coord = item.get<Poco::Redis::Array>(1);
    auto lng = coord.get<Poco::Redis::BulkString>(0).value();
    auto lat = coord.get<Poco::Redis::BulkString>(1).value();
    point.first = carId;
    point.second = std::make_pair(std::stod(lng), std::stod(lat));
    ret[i] = point;
  }

  return ret;
}
