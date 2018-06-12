#include <iostream>
#include <Poco/Util/Option.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerParams.h>

#include "geo-index-server-application.h"
#include "geo-index-request-handler-factory.h"

using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;

using Poco::Net::ServerSocket;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;


void GeoIndexServerApplication::initialize(Application& app) {
  loadConfiguration();
  ServerApplication::initialize(app);
}

void GeoIndexServerApplication::uninitialize() {
  ServerApplication::uninitialize();
}

void GeoIndexServerApplication::defineOptions(OptionSet& options)
{
  ServerApplication::defineOptions(options);

  options
    .addOption(
        Option("help", "h", "display argument help information")
          .required(false)
          .repeatable(false)
          .callback(OptionCallback<GeoIndexServerApplication>(this, &GeoIndexServerApplication::handleHelp))
    );
  options.addOption(
        Option("port", "p", "the port of the server")
          .required(false)
          .repeatable(false)
          .argument("PORT")
          .binding("port")
    );
}

void GeoIndexServerApplication::handleHelp(const std::string&, const std::string&) {
  _helpRequested = true;
  stopOptionsProcessing();
}

int GeoIndexServerApplication::main(const std::vector<std::string>& args)
{
  if (_helpRequested) {
    HelpFormatter helpFormatter(options());

    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("An HTTP server that maintains Geo Index using S2");
    helpFormatter.format(std::cout);
  } else {
    unsigned short port = (unsigned short)config().getInt("port", 8000);
    ServerSocket socket(port);
    HTTPServer server(new GeoIndexRequestHandlerFactory(m_registry), socket, new HTTPServerParams);
    server.start();
    waitForTerminationRequest();
    server.stop();
  }

  return Application::EXIT_OK;
}

int main(int argc, char** argv)
{
  GeoIndexServerApplication app;
  return app.run(argc, argv);
}
