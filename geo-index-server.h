#include <memory>
#include <vector>

#include <Poco/UUID.h>
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerParams.h"
#include <Poco/Util/ServerApplication.h>
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <s2/s2latlng.h>
#include <s2/s2point_index.h>

using Poco::Net::ServerSocket;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;

class GeoIndexServer {
  public:
    virtual void Write(const Poco::UUID &id, const S2LatLng &loc);
    virtual std::unique_ptr<const std::vector<Poco::UUID> > GetKNN(const S2LatLng &loc, int k);
  private:
    S2PointIndex<Poco::UUID> m_index;
};

class GeoIndexServerApplication: public ServerApplication {
  protected:
    void initialize(Application& self) {
      loadConfiguration();
      ServerApplication::initialize(self);
    }

    void uninitialize() {
      ServerApplication::uninitialize();
    }

    void defineOptions(OptionSet& options) {
      ServerApplication::defineOptions(options);
      Option helpOption =
        Option("help", "h", "display argument help information")
        .required(false)
        .repeatable(false)
        .callback(OptionCallback<GeoIndexServerApplication>(this, &GeoIndexServerApplication::handleHelp));

      options.addOption(helpOption);
    }

    void handleHelp(const std::string& name, const std::string& value) {
      HelpFormatter helpFormatter(options());
      helpFormatter.setCommand(commandName());
      helpFormatter.setUsage("OPTIONS");
      helpFormatter.setHeader("A web server that serves the current date and time.");
      helpFormatter.format(std::cout);
      stopOptionsProcessing();
      _helpRequested = true;
    }

    int main(const std::vector<std::string>& args) {
      unsigned short port = 8080;
      ServerSocket socket(port);
      HTTPServer server(, socket, new HTTPServerParams);
      server.start();
      waitForTerminationRequest();
      server.stop();
      return Application::EXIT_OK;
    }
};

