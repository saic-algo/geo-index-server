#ifndef __REQUEST_HANDLER__DELETE_INDEX__
#define __REQUEST_HANDLER__DELETE_INDEX__

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "base.h"

class DeleteIndexRequestHandler : public BaseRequestHandler {
  public:
    DeleteIndexRequestHandler(GeoIndexRegistry &registry, const std::string &uuid)
      : BaseRequestHandler("DeleteIndex", registry), m_uuid(uuid) { }
  public:
    void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
  private:
    const std::string m_uuid;
};

#endif // __REQUEST_HANDLER__DELETE_INDEX__

