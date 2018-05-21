#ifndef __REQUEST_HANDLER__CREATE_INDEX__
#define __REQUEST_HANDLER__CREATE_INDEX__

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "base.h"

class CreateIndexRequestHandler : public BaseRequestHandler {
  public:
    CreateIndexRequestHandler(GeoIndexRegistry &registry, const GeoIndexFactory &factory)
      : BaseRequestHandler("CreateIndex", registry), m_factory(factory) { }

  public:
    void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);

  private:
    const GeoIndexFactory &m_factory;
};

#endif // __REQUEST_HANDLER__CREATE_INDEX__

