#include <iostream>
#include "geo-index-request-handler.h"

using std::ostream;
using std::endl;

void GeoIndexRequestHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
  response.setChunkedTransferEncoding(true);
  response.setContentType("text");

  ostream &ostr = response.send();
  ostr << "Hello" << endl;
}

