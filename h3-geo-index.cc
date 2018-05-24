#include <memory>

#include "h3-geo-index.h"

using std::vector;
using std::make_unique;
using std::make_pair;

void H3GeoIndex::AddPoint(const GeoPoint &point) {
  GeoCoord coord = {
    .lat = degsToRads(point.latitude),
    .lon = degsToRads(point.longitude),
  };
  int idIndex = m_stringTable.size();
  m_stringTable.push_back(point.id);

  H3Index index = geoToH3(&coord, 10);
  //m_map.insert(make_pair(index, idIndex));
}

VGeoPointPtr H3GeoIndex::QueryClosestPoints(const GeoPoint &target, int maxCount, double maxRadius) const {
  return make_unique<VGeoPoint>();
}

