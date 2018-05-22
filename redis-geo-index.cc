#include <memory>
#include <Poco/Redis/Command.h> 

#include "redis-geo-index.h"

using std::make_unique;
using std::string;
using std::to_string;
using std::stod;

using Poco::Redis::Command;
using Poco::Redis::Array;
using Poco::Redis::BulkString;

RedisGeoIndex::~RedisGeoIndex() {
  Command cmd = Command::del(UUID());
  m_client.execute<Poco::Int64>(cmd);
}

void RedisGeoIndex::AddPoint(const GeoPoint &point) {
  Array cmd;
  const string &lat = to_string(point.latitude);
  const string &lng = to_string(point.longitude);

  cmd << "geoadd" << UUID() << lng << lat << point.id;

  m_client.execute<Poco::Int64>(cmd);
}

VGeoPointPtr RedisGeoIndex::QueryClosestPoints(const GeoPoint &target, int maxCount, double maxRadius) const {
  VGeoPointPtr result = make_unique<VGeoPoint>();

  double currentRadius = 100;
  Array resultTemp;
  const string lat = to_string(target.latitude);
  const string lng = to_string(target.longitude);
  const string radius = to_string(maxRadius);

  do {
    Poco::Redis::Array cmd;
    cmd << "georadius" << UUID() << lng << lat << radius << "m" << "ASC" << "WITHCOORD";

    resultTemp = const_cast<Poco::Redis::Client &>(m_client).execute<Array>(cmd);
  } while (resultTemp.size() < maxCount && currentRadius < maxRadius);

  for (int i = 0; i < maxCount && i < resultTemp.size(); ++i) {
    Array item = resultTemp.get<Array>(i);
    string id = item.get<BulkString>(0).value();
    Array coord = item.get<Array>(1);
    string lng = coord.get<BulkString>(0).value();
    string lat = coord.get<BulkString>(1).value();

    result->push_back(GeoPoint(id, stod(lat), stod(lng)));
  }
  // std::cout << resultTemp.toString() << std::endl;

  return result;
}

