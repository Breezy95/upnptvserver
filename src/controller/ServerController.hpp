#ifndef MyController_hpp
#define MyController_hpp



#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/core/data/stream/FileStream.hpp"
#include <unordered_map>
#include "../file_server/Utils.hpp"


#include OATPP_CODEGEN_BEGIN(ApiController) ///< Begin Codegen


class ServerController : public oatpp::web::server::api::ApiController {
		public:
		ServerController(const std::shared_ptr<ObjectMapper>& objectMapper)
		: oatpp::web::server::api::ApiController(objectMapper) 
		{}

    private:
		// we are going to use this for regular server functions outside of upnp

		OATPP_COMPONENT(std::shared_ptr<StaticFilesManager>, staticFileManager);
  		//OATPP_COMPONENT(std::shared_ptr<Playlist>, livePlaylist);


	public:

	/**
   *  Inject @objectMapper component here as default parameter
   */
  		static std::shared_ptr<ServerController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)){
    		return std::shared_ptr<ServerController>(std::make_shared<ServerController>(objectMapper));
  		}



ENDPOINT("HEAD", "/media/audio/{file}", headRequests,PATH(String, file)){
  //auto response = OutgoingResponse::createShared(Status::CODE_200, "");
   OATPP_LOGD("SERVER", "HEADER FILE");
  auto response = ResponseFactory::createResponse(Status::CODE_200);
  response->putHeader("Content-Type", "audio/mpeg");
  response->putHeader("ContentFeatures.DLNA.ORG", "DLNA.ORG_PN=MP3;DLNA.ORG_OP=01;DLNA.ORG_FLAGS=01700000000000000000000000000000");
  response->putHeader("Scid.DLNA.ORG", "839080694");
  response->putHeader("TransferMode.DLNA.ORG", "Streaming");
  response->putHeader("Content-Length", "463500");
  response->putHeader("Connection", "Keep-Alive");

  return response; 
}


ENDPOINT("GET", "/media/{folder}/{file}", getAfterHeadReq, PATH(String, foldername),PATH(String, filename), HEADER(String, rangeHeader, Header::RANGE)){
    
	auto range = oatpp::web::protocol::http::Range::parse(rangeHeader);
    OATPP_LOGD("Range", "start=%d, end=%d", range.start, range.end);

  auto body = std::make_shared<oatpp::web::protocol::http::outgoing::StreamingBody>(
	std::make_shared<oatpp::data::stream::FileInputStream>("media/audio/BisexualLight.mp3")
  );

  auto response = OutgoingResponse::createShared(Status::CODE_200, body);

  oatpp::web::protocol::http::ContentRange contentRange(oatpp::web::protocol::http::ContentRange::UNIT_BYTES,
                                                        range.start, range.end, 463500, true);

  response->putHeader(Header::CONTENT_RANGE, contentRange.toString());

  return response;
}
};

#include OATPP_CODEGEN_END(ApiController)


#endif /* ServerController_hpp */
