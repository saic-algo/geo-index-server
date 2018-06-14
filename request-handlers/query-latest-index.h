#ifndef __QUERY_LATEST_INDEX__
#define __QUERY_LATEST_INDEX__

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include"base.h"

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

class QueryLatestIndex :public BaseRequestHandler {
public:
	QueryLatestIndex(GeoIndexRegistry &registry) :BaseRequestHandler("QueryLatestIndex", registry) {}

	void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response);
};

#endif // !__QUERY_LATEST_INDEX__
