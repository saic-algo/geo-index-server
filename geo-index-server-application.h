#ifndef __GEO_INDEX_SERVER_APPLICATION__
#define __GEO_INDEX_SERVER_APPLICATION__

#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/OptionSet.h>
#include <iostream>

#include "geo-index-registry.h"
#include "s2-geo-index.h"

using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::OptionSet;


class GeoIndexServerApplication: public ServerApplication
{
  public:
    GeoIndexServerApplication():
      _helpRequested(false),
      m_registry(),
      m_factory() { }
    ~GeoIndexServerApplication() { }

  protected:
    void initialize(Application& self);
    void uninitialize();
    void defineOptions(OptionSet& options);
    void handleHelp(const std::string&, const std::string&);
    int main(const std::vector<std::string>& args);

  private:
    bool _helpRequested;
    GeoIndexRegistry m_registry;
    S2GeoIndexFactory m_factory;
};

#endif //__GEO_INDEX_SERVER_APPLICATION__ 

