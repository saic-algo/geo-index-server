#ifndef __S2_GEO_INDEX__
#define __S2_GEO_INDEX__

#include <s2/s2point_index.h>

#include "geo-index.h"

class S2GeoIndex : public GeoIndex {
  public:
    virtual void AddPoint(const GeoPoint &point);
    virtual VGeoPointPtr QueryClosestPoints(const GeoPoint &target, int maxCount, double maxRadius) const;

  private:
    S2PointIndex<std::string> m_pointIndex;
};

#endif // __S2_GEO_INDEX__
