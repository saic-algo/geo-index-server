#include <iostream>
#include <memory>
#include "create-index.h"

using std::istream;
using std::ostream;
using std::endl;
using std::make_unique;

void CreateIndexRequestHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
  response.setChunkedTransferEncoding(true);
  response.setContentType("text");

  istream &istm = request.stream();
  size_t length = request.getContentLength();
  auto buffer =  make_unique<char[]>(length);
  istm.read(buffer.get(), length);
  std::string content(buffer.get(), length);

  ostream &ostr = response.send();
  ostr << content << endl;
}

