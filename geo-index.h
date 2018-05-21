#ifndef __GEO_INDEX__
#define __GEO_INDEX__

#include <memory>
#include <vector>

#include <Poco/JSON/Array.h>

struct GeoPoint {
  double latitude;
  double longitude;
  std::string id;

  GeoPoint(const std::string &a_id, double a_latitude, double a_longitude)
    : id(a_id), latitude(a_latitude), longitude(a_longitude) { }

  GeoPoint(const Poco::JSON::Array::Ptr &pArr)
    : id(pArr->get(0).toString()), latitude(pArr->get(1)), longitude(pArr->get(2)) { }

  operator const Poco::JSON::Array::Ptr() const {
    Poco::JSON::Array::Ptr pArr(new Poco::JSON::Array);

    pArr->add(id);
    pArr->add(latitude);
    pArr->add(longitude);

    return pArr;
  }
};

typedef std::vector<GeoPoint> VGeoPoint;
typedef std::unique_ptr<VGeoPoint> VGeoPointPtr;

class GeoIndex {
  public:
    virtual void AddPoint(const GeoPoint &point) = 0;
    virtual VGeoPointPtr QueryClosestPoints(const GeoPoint &target, int maxCount, double maxRadius) const = 0;
};

class GeoIndexFactory {
  public:
    virtual std::unique_ptr<GeoIndex> CreateGeoIndex() const = 0;
};

template <typename TIndex>
class GeoIndexFactoryImpl : public GeoIndexFactory {
  public:
    virtual std::unique_ptr<GeoIndex> CreateGeoIndex() const {
      return std::make_unique<TIndex>();
    }
};

#endif // __GEO_INDEX__

