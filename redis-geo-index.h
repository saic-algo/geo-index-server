#ifndef __REDIS_GEO_INDEX__
#define __REDIS_GEO_INDEX__

#include <Poco/Redis/Client.h>

#include "geo-index.h"

class RedisGeoIndex : public GeoIndex {
  public:
    RedisGeoIndex(const char *host, unsigned short port) : GeoIndex(), m_client(host, port) { }
    ~RedisGeoIndex();
      
  public:
    virtual void AddPoint(const GeoPoint &point);
    virtual VGeoPointPtr QueryClosestPoints(const GeoPoint &target, int maxCount, double maxRadius) const;

  private:
    Poco::Redis::Client m_client;
};

#endif // __S2_GEO_INDEX__

