#include <iostream>
#include "create-index.h"

using std::ostream;
using std::endl;

void CreateIndexRequestHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
  response.setChunkedTransferEncoding(true);
  response.setContentType("text");

  ostream &ostr = response.send();
  ostr << "Create Index" << endl;
}

