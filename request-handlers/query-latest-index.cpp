#include"query-latest-index.h"
//#include"request-handlers\query-latest-index.h"
#include"omp.h"
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

using Poco::JSON::Object;
using Poco::JSON::Array;

void QueryLatestIndex::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {

	const GeoIndex *pIndex=m_registry.GetGeoIndex();

	if (!pIndex) {
		BadRequest(response);
		return;
	}

	const Object::Ptr object=ReadRequestBody(request);

	const Array::Ptr targets=object->get("points").extract<Array::Ptr>();
	const int count=object->get("count");
	const double radius=object->get("radius");

	Object::Ptr objres(new Object);
	Array::Ptr results(new Array);
	std::vector<Object> ret(targets->size());

	int num_threads = 16;
	omp_set_num_threads(num_threads);
	std::cout << "Max num threads: " << omp_get_max_threads() << ", Num threads: " << omp_get_num_threads() << ", thread id " << omp_get_thread_num() << std::endl;
	double starttime = omp_get_wtime();

#pragma omp parallel for num_threads(num_threads)
	for (int i = 0; i < (int)targets->size(); ++i) {
		Object::Ptr result(new Object);
		Array::Ptr points(new Array);

		Array::Ptr targetPoint = targets->get(i).extract<Array::Ptr>();

		const std::string &id = targetPoint->get(0).toString();
		const double lat = (double)targetPoint->get(1);
		const double lng = (double)targetPoint->get(2);

		auto pPoints = pIndex->QueryClosestPoints(GeoPoint(id, lat, lng), count, radius);

		for (auto &point : *pPoints) points->add((const Array::Ptr)point);

		ret[i].set("Points", points);
	}
	for(int i=0;i<(int)targets->size();++i) results->add(ret[i]);
	double endtime=omp_get_wtime();
	std::cout << "Using " << num_threads << " threads. Query starts at " << starttime << ", finishes at " << endtime << ", duration " << endtime - starttime << " seconds." << std::endl;
	
	objres->set("results",results);
	SendResponse(response,objres);
}