#ifndef __GEO_INDEX_REGISTRY__
#define __GEO_INDEX_REGISTRY__

#include <memory>
#include <string>
#include <unordered_map>
#include <Poco/UUIDGenerator.h>

#include "geo-index.h"

class GeoIndexRegistry {
  public:
    void AddGeoIndex(std::unique_ptr<GeoIndex> index) {
			m_list.push_back(index.get());
			map_list[index->UUID()]=m_list.begin();
      m_map.insert(std::make_pair(index->UUID(), move(index)));
    }

    const GeoIndex *GetGeoIndex(const std::string &uuid) const {
			if(m_map.count(uuid))
	      return m_map.find(uuid)->second.get();//May not exist
			return nullptr;
    }

		const GeoIndex* GetGeoIndex()const {
			if(m_list.empty()) return nullptr;
			auto it=m_list.begin();
			return *it;
		}

		void DelGeoIndex(const std::string &uuid) {
			if (m_map.count(uuid)) {
				m_list.erase(map_list[uuid]);
				map_list.erase(uuid);
				m_map.erase(uuid);
			}
		}

		void DelGeoIndex() {
			if (!m_list.empty()) {
				auto it = m_list.begin();
				map_list.erase((*it)->UUID());
				m_map.erase((*it)->UUID());
				m_list.erase(it);
			}
		}
  private:
    std::unordered_map<std::string, std::unique_ptr<GeoIndex>> m_map;
		std::list<GeoIndex*> m_list;
		std::unordered_map<std::string,std::list<GeoIndex*>::iterator> map_list;
};

#endif // __GEO_INDEX_REGISTRY__

