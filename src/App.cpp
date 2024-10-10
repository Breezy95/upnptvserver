#include "controller/ServerController.hpp"
#include "controller/UpnpController.hpp"
#include "./AppComponent.hpp"

#include "oatpp/network/Server.hpp"
#include "upnp.h"
#include <iostream>


void run() {
  
  AppComponent components; // Create scope Environment components
  
  /* create ApiControllers and add endpoints to router */
  auto router = components.httpRouter.getObject();

  router->addController(ServerController::createShared());
  router->addController(UpnpController::createShared());
  
  /* create server */
  oatpp::network::Server server(components.serverConnectionProvider.getObject(),
                                components.serverConnectionHandler.getObject());
  
  OATPP_LOGI("Server", "Running on port %s...", components.serverConnectionProvider.getObject()->getProperty("port").toString()->c_str());
  
  server.run();
  
}


int main(int argc, const char * argv[]) {

  oatpp::base::Environment::init();

  run();  
  oatpp::base::Environment::destroy();
  
  return 0;
};