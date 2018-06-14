//#include"request-handlers/delete-latest-index.h"
#include"delete-latest-index.h"

void DeleteLatestIndex::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
	m_registry.DelGeoIndex();

	Poco::JSON::Object::Ptr result(new Poco::JSON::Object);
	SendResponse(response,result);
}