#include "UpnpController.hpp"
#include "Utils.hpp"
std::map<int,std::string> methodmapping {
    {ALL,"ssdp:all"},
    {ROOT,":rootdevice"},
    {UUID, "uuid:"},
    {URNDEVICE, "urn:device:"},
    {URNSERVICE, "urn:service:"},
};
std::shared_ptr<UpnpController::OutgoingResponse> UpnpController::getRangeResponse(const oatpp::String& rangeStr,
                                                                                     const oatpp::String& file) const
{

  auto range = oatpp::web::protocol::http::Range::parse(rangeStr.getPtr());

  if(range.end == 0) {
    range.end = file->size() - 1;
  }

  OATPP_ASSERT_HTTP(range.isValid() &&
                    range.start < file->size() &&
                    range.end > range.start &&
                    range.end < file->size(), Status::CODE_416, "Range is invalid");

  auto chunk = oatpp::String(&file->data()[range.start], (v_int32)(range.end - range.start + 1));

  auto response = createResponse(Status::CODE_206, chunk);

  oatpp::web::protocol::http::ContentRange contentRange(oatpp::web::protocol::http::ContentRange::UNIT_BYTES,
                                                        range.start, range.end, file->size(), true);

  OATPP_LOGD("Server", "range=%s \n", contentRange.toString()->c_str());

  response->putHeader(Header::CONTENT_RANGE, contentRange.toString());
  return response;

}

std::shared_ptr<UpnpController::OutgoingResponse> UpnpController::getStaticFileResponse(const oatpp::String& filename,
                                                                                          const oatpp::String& rangeHeader) const
{

  auto file = this->m_staticFileManager->getFile(filename);

  OATPP_ASSERT_HTTP(file.get() != nullptr, Status::CODE_404, "File not found");

  std::shared_ptr<OutgoingResponse> response;

  if(!rangeHeader) {
    response = getFullFileResponse(file);
  } else {
    response = getRangeResponse(rangeHeader, file);
  }

  response->putHeader("Accept-Ranges", "bytes");
  response->putHeader(Header::CONNECTION, Header::Value::CONNECTION_KEEP_ALIVE);
  auto mimeType = staticFileManager->guessMimeType(filename);
  if(mimeType) {
    response->putHeader(Header::CONTENT_TYPE, mimeType);
  } else {
    OATPP_LOGD("Server", "Unknown Mime-Type. Header not set");
  }

  return response;

}

std::shared_ptr<UpnpController::OutgoingResponse> UpnpController::getFullFileResponse(const oatpp::String& file) const {

  return createResponse(Status::CODE_200, file);
}

int UpnpController::PlaybackActionHandler(Object<AVTransportDTO> dto){
  //get device info
    // get actionurl
    // args
    const char* actionUrl = "http://192.168.0.100:52235/upnp/control/AVTransport1";
    // "urn:schemas-upnp-org:service:AVTransport:1"
    OATPP_LOGI("playbackhandler", "before mapping args");
    std::map<std::string, std::string> args = {
        {"InstanceID",dto->instanceID},
        {"CurrentURI",dto->currentURI},
        {"CurrentURIMetaData", generateURIMetadata(dto)}
    };
    //args list should be of type oatpp::List<oatpp::Object<String>>

        for (const auto& fieldMap : *dto->argList) {
        // `fieldMap` is of type `oatpp::Fields<KVPairDTO>`
           OATPP_LOGI("KVPair"," Key: %s, Value: %s\n", fieldMap->key->c_str(), fieldMap->value->c_str());
        }

    struct deviceInfo dev = {dto->device->ID, dto->device->type, dto->device->location};
    auto exists = [&](const deviceInfo &dev){
    return std::find_if(std::begin(upnpManager->known_devices),std::end(upnpManager->known_devices), [&](const deviceInfo &dInfo) { return (dInfo.id == dev.id && dInfo.location == dev.location && dInfo.type == dev.type); }) != std::end(upnpManager->known_devices); 
    };

    if(dev.id.compare("") !=0 && dev.type.compare("")!=0 && dev.location.compare("") != 0 && !exists(dev)){
      upnpManager->known_devices.push_back(dev);
    }
    
  IXML_Document* actionDoc = this->upnpManager->generatePlayBackActionDoc(dto->action->c_str(),dto->servicetype->c_str(), args);
  //gen action url?
  String actionURL = "";
  this->upnpManager->sendUpnpAction(actionUrl, dto->servicetype->c_str(),actionDoc);
  return 1;
};



std::string UpnpController::generateURIMetadata(oatpp::web::server::api::ApiController::Object<AVTransportDTO> dto){
std::string uri = dto->currentURI->c_str();
// URI exmple http://192.168.0.212:8000/media/audio/BisexualLight.mp3"
oatpp::network::Url parsed_uri = oatpp::network::Url::Parser::parseUrl(uri); 
OATPP_LOGI("GENERATE URI","uri: %s,\tparsed uri:%s\n",uri.c_str(), parsed_uri.path->substr(1).c_str());
OATPP_LOGI("","action %s",dto->action->c_str());

 // Allocate format context
    AVFormatContext* formatContext = avformat_alloc_context();

    // Open media file
    if (avformat_open_input(&formatContext, parsed_uri.path->substr(1).c_str(), nullptr, nullptr) != 0) {
        std::cerr << "Error: Couldn't open file." << std::endl;
        return "";
    }

    // Retrieve stream information
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        std::cerr << "Error: Couldn't find stream information." << std::endl;
        avformat_close_input(&formatContext);
        return "";
    }

    // Print format details
    std::cout << "Format: " << formatContext->iformat->name << std::endl;
    std::cout << "Duration: " << formatContext->duration / AV_TIME_BASE << " seconds" << std::endl;
    std::cout << "Bit rate: " << formatContext->bit_rate << " bps" << std::endl;

    // Print stream details
    for (unsigned int* i = 0; *i < formatContext->nb_streams; i++) {
        std::cout << "\nStream " << i << ": " << std::endl;
        AVStream* stream = formatContext->streams[*i];
        AVCodecParameters* codecParams = stream->codecpar;


        std::cout << "Codec Type: " << av_get_media_type_string(codecParams->codec_type) << std::endl;

        if (codecParams->codec_type == AVMEDIA_TYPE_VIDEO) {
            std::cout << "Resolution: " << codecParams->width << "x" << codecParams->height << std::endl;
            std::cout << "Frame Rate: " << av_q2d(stream->avg_frame_rate) << " fps" << std::endl;
        } else if (codecParams->codec_type == AVMEDIA_TYPE_AUDIO) {
            std::cout << "Sample Rate: " << codecParams->sample_rate << std::endl;
            std::cout << "Channels: " << codecParams->ch_layout.nb_channels << std::endl;
        }

        const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
        if (codec) {
            std::cout << "Codec: " << codec->name << std::endl;
        } else {
            std::cout << "Unknown codec." << std::endl;
        }
    }

    // Close input and clean up
    avformat_close_input(&formatContext);
    avformat_free_context(formatContext);

//need uri, filetype that will match with what the tv wants, whether its audio or video
//createMetadataDocument(uri, this->staticFileManager);
return "asa";
}

// read info from dto to get file type
// figure out how to get metadata from files
// figure out how to get multiple configs for tv