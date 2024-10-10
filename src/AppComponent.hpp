#ifndef AppComponent_hpp
#define AppComponent_hpp

#include "Utils.hpp"

#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

/**
 *  Class which creates and holds Application components and registers components in oatpp::base::Environment
 *  Order of components initialization is from top to bottom
 */
class AppComponent {
public:
 
/**
   *  Create ConnectionProvider component which listens on the port
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([] {
    /* non_blocking connections should be used with AsyncHttpConnectionHandler for AsyncIO */
    return oatpp::network::tcp::server::ConnectionProvider::createShared({"0.0.0.0", 8001, oatpp::network::Address::IP_4});
  }());
  
  /**
   *  Create Router component
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] {
    return oatpp::web::server::HttpRouter::createShared();
  }());


  /**
   *  Create ConnectionHandler component which uses Router component to route requests
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)([] {
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router); // get Router component
    return oatpp::web::server::HttpConnectionHandler::createShared(router);
  }());

 /**
   *  Create ObjectMapper component to serialize/deserialize DTOs in Contoller's API
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)([] {
    auto serializerConfig = oatpp::parser::json::mapping::Serializer::Config::createShared();
    auto deserializerConfig = oatpp::parser::json::mapping::Deserializer::Config::createShared();
    deserializerConfig->allowUnknownFields = false;
    auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared(serializerConfig, deserializerConfig);
    return objectMapper;
  }());
  
  OATPP_CREATE_COMPONENT(std::shared_ptr<StaticFilesManager>, staticFilesManager)([] {
    return std::make_shared<StaticFilesManager>((std::filesystem::current_path() / "media").c_str()); 
  }());

   OATPP_CREATE_COMPONENT(std::shared_ptr<UPnPDeviceManager>, upnpManager)([] {
    return std::make_shared<UPnPDeviceManager>();
  }());
};  

#endif