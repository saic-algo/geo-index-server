#include <memory>
#include <Poco/Redis/Command.h> 

#include "redis-geo-index.h"

using std::make_unique;
using std::string;
using std::to_string;

using Poco::Redis::Command;
using Poco::Redis::Array;

RedisGeoIndex::~RedisGeoIndex() {
  Command cmd = Command::del(UUID());
  m_client.execute<Poco::Int64>(cmd);
}

void RedisGeoIndex::AddPoint(const GeoPoint &point) {
  Array cmd;
  const string &lat = to_string(point.latitude);
  const string &lng = to_string(point.longitude);

  cmd << "geoadd" << m_uuid << lat << lng << point.id;

  std::cout << cmd.toString() << std::endl;

  m_client.execute<Poco::Int64>(cmd);
}

VGeoPointPtr RedisGeoIndex::QueryClosestPoints(const GeoPoint &target, int maxCount, double maxRadius) const {
  VGeoPointPtr result = make_unique<VGeoPoint>();

  return result;
}

