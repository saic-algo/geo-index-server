#ifndef __GEO_INDEX_REGISTRY__
#define __GEO_INDEX_REGISTRY__

#include <memory>
#include <string>
#include <unordered_map>
#include <Poco/UUIDGenerator.h>

#include "geo-index.h"

class GeoIndexRegistry {
  public:
    const std::string AddGeoIndex(std::unique_ptr<GeoIndex> index) {
      Poco::UUID uuid = Poco::UUIDGenerator::defaultGenerator().create();

      m_map.insert(std::make_pair(uuid.toString(), move(index)));

      return uuid.toString();
    }

    const GeoIndex *GetGeoIndex(const std::string &uuid) const {
      return m_map.find(uuid)->second.get();
    }

    void DelGeoIndex(const std::string &uuid) {
      m_map.erase(uuid);
    }

    
  private:
    std::unordered_map<std::string, std::unique_ptr<GeoIndex>> m_map;
};

#endif // __GEO_INDEX_REGISTRY__

