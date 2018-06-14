#ifndef __DELETE_LATEST__INDEX__
#define __DELETE_LATEST__INDEX__

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "base.h"

class DeleteLatestIndex :public BaseRequestHandler {
public:
	DeleteLatestIndex(GeoIndexRegistry &registry) :BaseRequestHandler("DeleteLatestIndex", registry) {}

	void handleRequest(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &);
};

#endif // !__DELETE_LATEST__INDEX__
