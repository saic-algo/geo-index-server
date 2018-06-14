//#include"request-handlers/active-create-index.h"
#include"active-create-index.h"

#include<memory>
#include "s2-geo-index.h"
#include "redis-geo-index.h"
#include "h3-geo-index.h"
using Poco::JSON::Object;
using Poco::JSON::Array;

#define PV(name) std::cout<<#name" = "<<name<<std::endl;

void active_create_index(GeoIndexRegistry *registry) {
	Poco::URI uri(std::string("http://127.0.0.1:8080/GPS-server/cars/"));

	Poco::Net::HTTPClientSession session;
	Poco::Net::HTTPRequest request;
	Poco::Net::HTTPResponse response;

	session.setHost(uri.getHost());
	session.setPort(uri.getPort());

	request.setHost(uri.getHost());
	request.setURI(uri.getPathAndQuery());
	request.setMethod(Poco::Net::HTTPRequest::HTTP_GET);

	PV(uri.getHost());
	PV(uri.getPathAndQuery());
	
	for (;;) {
		std::cout<<"try to pull data"<<std::endl;
		session.sendRequest(request);
		std::istream &is=session.receiveResponse(response);
		const Poco::JSON::Object::Ptr object=Poco::JSON::Parser().parse(is).extract<Poco::JSON::Object::Ptr>();
		Poco::JSON::Array::Ptr points=object->get("points").extract<Array::Ptr>();

		//not define index type default S2
		std::unique_ptr<GeoIndex> index;
		index=std::make_unique<S2GeoIndex>();

		for (int i = 0; i < (int)points->size(); ++i) {
			Array::Ptr point = points->get(i).extract<Array::Ptr>();
			const std::string &id = point->get(0).toString();
			const double lat = (double)point->get(1);
			const double lng = (double)point->get(2);

			index->AddPoint(GeoPoint(id, lat, lng));
		}

		registry->AddGeoIndex(move(index));

		std::chrono::milliseconds dura(5000);
		std::this_thread::sleep_for(dura);
	}
}