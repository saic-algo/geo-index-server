#ifndef __REDIS_GEO_INDEX__
#define __REDIS_GEO_INDEX__

#include <iostream>

#include <Poco/Redis/Command.h>
#include <Poco/Redis/Client.h>

#include "geo-index.h"

class RedisGeoIndex : public GeoIndex {
  public:
    RedisGeoIndex(const char *host, unsigned short port) : GeoIndex(), m_client(host, port) {
      // Poco::Redis::Command cmd = Poco::Redis::Command::set("mykey", "Hello");

      // std::cout << cmd.toString() << std::endl;

      // std::cout << m_client.execute<Poco::Redis::BulkString>(cmd).value() << std::endl;
      // std::cout << m_client.execute<std::string>(cmd) << std::endl;
    }
    ~RedisGeoIndex();
      
  public:
    virtual void AddPoint(const GeoPoint &point);
    virtual VGeoPointPtr QueryClosestPoints(const GeoPoint &target, int maxCount, double maxRadius) const;

  private:
    Poco::Redis::Client m_client;
};

#endif // __S2_GEO_INDEX__

