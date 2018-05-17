#include <iostream>
#include <s2/s2closest_point_query.h>

#include "geo-index-server.h"

void GeoIndexServer::Write(const Poco::UUID &id, const S2LatLng &loc) {
  this->m_index.Add(loc.ToPoint(), id);
}

std::unique_ptr<const std::vector<Poco::UUID> > GeoIndexServer::GetKNN(const S2LatLng &loc, int k) {
  S2ClosestPointQuery<Poco::UUID>::PointTarget target(loc.ToPoint());
  S2ClosestPointQuery<Poco::UUID> query(&this->m_index);
  query.mutable_options()->set_max_points(k);

  auto result = query.FindClosestPoints(&target);

  std::cout << result.size() << std::endl;

  return NULL;
}

