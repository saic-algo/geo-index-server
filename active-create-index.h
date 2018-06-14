#ifndef __ACTIVE_CREATE_INDEX__
#define __ACTIVE_CREATE_INDEX__

#include"geo-index-registry.h"
#include<iostream>
#include<chrono>
#include<thread>
#include<Poco/URI.h>
#include<Poco/Net/HTTPResponse.h>
#include<Poco/Net/HTTPRequest.h>
#include<Poco/Net/HTTPClientSession.h>
#include<Poco/JSON/JSON.h>
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/JSONException.h"
#include "Poco/StreamCopier.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Query.h"
#include "Poco/JSON/PrintHandler.h"

void active_create_index(GeoIndexRegistry *registry);
#endif // !__ACTIVE_CREATE_INDEX__
