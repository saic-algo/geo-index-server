#ifndef __GEO_INDEX_REQUEST_HANDLER_FACTORY__
#define __GEO_INDEX_REQUEST_HANDLER_FACTORY__

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>

#include "geo-index.h"

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServerRequest;
using Poco::Redis::Client;

class GeoIndexRequestHandlerFactory : public HTTPRequestHandlerFactory {
  public:
    GeoIndexRequestHandlerFactory(std::shared_ptr<GeoIndexRegistry> registry, std::shared_ptr<Client> client)
     : m_registry(registry),
       m_redisClient(client) { }
       
  public:
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request);
  private:
    std::shared_ptr<GeoIndexRegistry> m_registry;
    std::shared_ptr<Client> m_redisClient;
};


#endif // __GEO_INDEX_REQUEST_HANDLER__
