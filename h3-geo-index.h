#ifndef __H3_GEO_INDEX__
#define __H3_GEO_INDEX__

#include <vector>
#include <map>
// #include <unordered_map>

#include <h3/h3api.h>

#include "geo-index.h"

class H3GeoIndex : public GeoIndex {
  public:
    virtual void AddPoint(const GeoPoint &point);
    virtual VGeoPointPtr QueryClosestPoints(const GeoPoint &target, int maxCount, double maxRadius) const;
  private:
    // std::unordered_map<H3Index, std::vector<int> > m_indices[16];
    std::multimap<H3Index, int> m_map;
    std::vector<std::string> m_stringTable;
};

#endif // __H3_GEO_INDEX__

