#ifndef __GEO_INDEX_REQUEST_HANDLER_FACTORY__
#define __GEO_INDEX_REQUEST_HANDLER_FACTORY__

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>

#include "geo-index-registry.h"

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServerRequest;

class GeoIndexRequestHandlerFactory : public HTTPRequestHandlerFactory {
  public:
    GeoIndexRequestHandlerFactory(GeoIndexRegistry &registry, const GeoIndexFactory &factory)
      : m_registry(registry), m_factory(factory) { }
  public:
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request);
  private:
    GeoIndexRegistry &m_registry;
    const GeoIndexFactory &m_factory;
};


#endif // __GEO_INDEX_REQUEST_HANDLER__
