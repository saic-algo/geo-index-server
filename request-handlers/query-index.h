#ifndef __REQUEST_HANDLER__QUERY_INDEX__
#define __REQUEST_HANDLER__QUERY_INDEX__

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "base.h"

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

class QueryIndexRequestHandler : public BaseRequestHandler {
  public:
    QueryIndexRequestHandler(GeoIndexRegistry &registry, const std::string &uuid)
      : BaseRequestHandler("QueryIndex", registry), m_uuid(uuid) { }
  public:
    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response);
  private:
    const std::string m_uuid;
};

#endif // __REQUEST_HANDLER__QUERY_INDEX__

