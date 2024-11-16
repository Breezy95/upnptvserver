#ifndef UpnpController_hpp
#define UpnpController_hpp


#include "../file_server/Utils.hpp"

#include "upnp/upnp.h"
#include "upnp/UpnpString.h"
#include "upnp/UpnpDiscovery.h" 
#include <iostream>
#include <map>
#include <filesystem>
#include "upnp/upnptools.h"
#include "upnp/ixml.h"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

#include "../DTO/upnpcontrollerDTO.cpp"


namespace fs = std::filesystem;

enum searchTargets{
	ALL = 0,
    ROOT,
	UUID,
    URN,
    URNDEVICE,
    URNSERVICE
};

extern std::map<int,std::string> methodmapping;




class UpnpController : public oatpp::web::server::api::ApiController {
public:
UpnpController(const std::shared_ptr<ObjectMapper>& objectMapper)
: oatpp::web::server::api::ApiController(objectMapper) 
{}

private:
OATPP_COMPONENT(std::shared_ptr<StaticFilesManager>, staticFileManager);
OATPP_COMPONENT(std::shared_ptr<UPnPDeviceManager>, upnpManager);

private:
    std::shared_ptr<UPnPDeviceManager> m_upnpManager;
    std::shared_ptr<StaticFilesManager> m_staticFileManager;
	int initUpnpSuccess; 
	int clientSuccess;
    int serverSuccess;

private:
    std::string generateURIMetadata(oatpp::web::server::api::ApiController::Object<AVTransportDTO> dto);
	std::shared_ptr<OutgoingResponse> getStaticFileResponse(const oatpp::String& filename, const oatpp::String& rangeHeader) const;
	std::shared_ptr<OutgoingResponse> getFullFileResponse(const oatpp::String& file) const;
	std::shared_ptr<OutgoingResponse> getRangeResponse(const oatpp::String& rangeStr, const oatpp::String& file) const;

public:
    int PlaybackActionHandler(Object<AVTransportDTO> dto);
    static std::shared_ptr<UpnpController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)){
    		return std::shared_ptr<UpnpController>(std::make_shared<UpnpController>(objectMapper));
  	}


#include OATPP_CODEGEN_BEGIN(ApiController)

ENDPOINT("POST", "/search/{target}", searchForTarget,PATH(Int64, target)){
    this->upnpManager->startDeviceDiscoveryByTarget(methodmapping.at(target));
    return createResponse(Status::CODE_200, "searching for target:" + target);
}


ENDPOINT("POST", "/play/{typing}", playAction, PATH(String, typing), BODY_DTO(Object<AVTransportDTO>, transportDTO)){ 
    PlaybackActionHandler(transportDTO);
    return createResponse(Status::CODE_200, "Play Request sent");
}



ENDPOINT("POST", "/advertise", advertiseDevice){
    this->upnpManager->advertiseDevice();
    return createResponse(Status::CODE_200);
}

ENDPOINT("POST", "/list/devices", listKnownDevices){
    std::string devices;

    for (const auto& device : upnpManager->known_devices) {
    devices += device.id + "  ";
}


    return createResponse(Status::CODE_200, devices);
}




#include OATPP_CODEGEN_END(ApiController)

};
#endif



