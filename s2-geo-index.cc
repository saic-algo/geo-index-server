#include <s2/s2latlng.h>
#include <s2/s2closest_point_query.h>
#include <s2/s2earth.h>

#include "s2-geo-index.h"

using std::string;
using std::unique_ptr;
using std::make_unique;

void S2GeoIndex::AddPoint(const GeoPoint &point) {
  S2LatLng loc = S2LatLng::FromDegrees(point.latitude, point.longitude);
  m_pointIndex.Add(loc.ToPoint(), point.id);
}

VGeoPointPtr S2GeoIndex::QueryClosestPoints(const GeoPoint &target, int maxCount, double maxRadius) const {
  VGeoPointPtr points(new VGeoPoint);

  S2LatLng loc = S2LatLng::FromDegrees(target.latitude, target.longitude);
  typename S2ClosestPointQuery<string>::PointTarget s2target(loc.ToPoint());
  S2ClosestPointQuery<string> s2query(&m_pointIndex);

  S1Angle maxRadiusAng = S1Angle::Radians(S2Earth::KmToRadians(maxRadius));

  s2query.mutable_options()->set_max_points(maxCount);
  s2query.mutable_options()->set_max_distance(maxRadiusAng);

  auto results = s2query.FindClosestPoints(&s2target);

  for (auto &result: results) {
    S2LatLng loc(result.point());

    points->push_back(GeoPoint(result.data(), loc.lat().degrees(), loc.lng().degrees()));
  }

  return points;
}

