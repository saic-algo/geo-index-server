#ifndef __GEO_INDEX__
#define __GEO_INDEX__

#include <memory>
#include <string>
#include <unordered_map>
#include <s2/s2point_index.h>

typedef S2PointIndex<std::string> GeoIndex;
typedef std::unordered_map<std::string, std::unique_ptr<GeoIndex> > GeoIndexRegistry;

#endif // __GEO_INDEX__
