#include <unistd.h>
#include "Utils.hpp"

#include <fstream>
#include <cstring>
#include <cstdarg>
#include <iostream>
#include <oatpp/core/Types.hpp>
#include "oatpp/web/server/api/ApiController.hpp"
#include <oatpp/core/base/Environment.hpp>


const char* base_desc = R"xml(<?xml version="1.0"?>
<root xmlns="urn:schemas-upnp-org:device-1-0">
  <specVersion>
    <major>1</major>
    <minor>0</minor>
  </specVersion>
  <device>
    <deviceType>urn:schemas-upnp-org:device:MediaRenderer:1</deviceType>
    <friendlyName>Sample Media Renderer</friendlyName>
    <manufacturer>Sample Manufacturer</manufacturer>
    <manufacturerURL>http://www.sample.com</manufacturerURL>
    <modelDescription>Sample Media Renderer</modelDescription>
    <modelName>SampleModel</modelName>
    <modelNumber>1.0</modelNumber>
    <modelURL>http://www.sample.com/model</modelURL>
    <serialNumber>123456789</serialNumber>
    <UDN>uuid:sample-uuid</UDN>
    <serviceList>
      <service>
        <serviceType>urn:schemas-upnp-org:service:ContentDirectory:1</serviceType>
        <serviceId>urn:upnp-org:serviceId:ContentDirectory</serviceId>
        <SCPDURL>/content-directory.xml</SCPDURL>
        <controlURL>/control/content-directory</controlURL>
        <eventSubURL>/event/content-directory</eventSubURL>
      </service>
    </serviceList>
  </device>
</root>)xml";


int getMetadata(std::string filepath){
 // Open input file and allocate format context
     
  AVFormatContext *format_ctx = NULL;
  const AVDictionaryEntry *tag = NULL;

  
  // Find video stream
    int video_stream_idx = -1;
    for (unsigned int i = 0; i < format_ctx->nb_streams; ++i) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }

    if (video_stream_idx == -1) {
        std::cerr << "Could not find video stream" << std::endl;
        return 4;
    }

    // Get codec parameters and codec context
    AVCodecParameters* codecpar = format_ctx->streams[video_stream_idx]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codecpar);
    avcodec_free_context(&codec_ctx);
// tmr  we test out a video file, 

return 0;
}



int upnpServerCallback(Upnp_EventType eventType, const void* event, void* cookie){
OATPP_LOGI("servercallback",  "advertisement alive, error code: %s", Upnp_EventType_e::UPNP_DISCOVERY_ADVERTISEMENT_ALIVE);
   std::cout << "EventType: " << eventType << std::endl;
    switch (eventType) {
        case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE:{
        
        break;
        }
        case UPNP_DISCOVERY_SEARCH_RESULT: {
            const UpnpDiscovery* discoveryEvent = (UpnpDiscovery*)(event);
            const char *deviceId = UpnpDiscovery_get_DeviceID_cstr(discoveryEvent);
            const char *deviceType = UpnpDiscovery_get_DeviceType_cstr(discoveryEvent);
            const char *location = UpnpDiscovery_get_Location_cstr(discoveryEvent);
            //results->push_back((deviceInfo){deviceId,deviceType,location});
            const char * date = UpnpDiscovery_get_Date_cstr(discoveryEvent);
            std::cout << "Search result: " << std::endl;
            std::cout << deviceId << std::endl;
            std::cout << location << std::endl;
            std::cout << deviceType << std::endl;
            break;
        }
        default:{
            std::cout << "Other event received: " << eventType << std::endl;
            break;
        }
    }
    return 1;

}

int upnpClientCallback(Upnp_EventType eventType, const void* event, void* cookie) {   
  
    switch (eventType) {
        case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE: {
            //pass in a pointer to known_devices?
            // cast here will change appear
            const UpnpDiscovery* discoveryEvent = static_cast<const UpnpDiscovery*>(event);
            const char* deviceId = UpnpDiscovery_get_DeviceID_cstr(discoveryEvent);
            const char *deviceType = UpnpDiscovery_get_DeviceType_cstr(discoveryEvent);
            const char *location = UpnpDiscovery_get_Location_cstr(discoveryEvent);
            const char * date = UpnpDiscovery_get_Date_cstr(discoveryEvent);
            OATPP_LOGI("UPNP_DISC_ALIVE", "\tdevice_ID: %s\ndeviceType: %s\nlocation: %s\ndate: %s", deviceId, deviceType,location,date);
            break;
        }
        case UPNP_DISCOVERY_SEARCH_RESULT: {
            std::vector<deviceInfo>* cook = (std::vector<deviceInfo>*)(cookie);
            const UpnpDiscovery* discoveryEvent = static_cast<const UpnpDiscovery*>(event);
            const char* deviceId = UpnpDiscovery_get_DeviceID_cstr(discoveryEvent);
            const char *deviceType = UpnpDiscovery_get_DeviceType_cstr(discoveryEvent);
            const char *location = UpnpDiscovery_get_Location_cstr(discoveryEvent);
            const char * date = UpnpDiscovery_get_Date_cstr(discoveryEvent);
            if(cook == nullptr){
              OATPP_LOGE("UPNP CLIENT CALLBACK", "COOKIE INVALID");
            }
            else{
              if(cook->empty()){
              cook->push_back({deviceId, deviceType, location});
              OATPP_LOGI("SUCCESSFUL SEARCH RESULT", "device id: %s\t, device type: %s\t, device location: %s\n", cook->at(0).id, cook->at(0).type, cook->at(0).location);
              }
            }
              
            std::cout << "Search result: "<< deviceId << std::endl << deviceType << std::endl;
            break;
        }
        
        case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE: {
            const UpnpDiscovery* discoveryEvent = static_cast<const UpnpDiscovery*>(event);
            std::cout << "Device removed: " << discoveryEvent << std::endl;
            const char* deviceId = UpnpDiscovery_get_DeviceID_cstr(discoveryEvent);
            const char *deviceType = UpnpDiscovery_get_DeviceType_cstr(discoveryEvent);
            const char *location = UpnpDiscovery_get_Location_cstr(discoveryEvent);
            const char * date = UpnpDiscovery_get_Date_cstr(discoveryEvent);
            
            std::cout << "Search result: "<< deviceId << std::endl << deviceType << std::endl;
            break;
        }
        case UPNP_EVENT_SUBSCRIPTION_REQUEST: {
            const UpnpSubscriptionRequest* subscriptionRequest = static_cast<const UpnpSubscriptionRequest*>(event);
            std::cout << "Subscription request from: " << UpnpSubscriptionRequest_get_ServiceId_cstr(subscriptionRequest)<< std::endl;
            break;
        }
        case UPNP_CONTROL_ACTION_REQUEST: {
            const UpnpActionRequest* actionRequest = static_cast<const UpnpActionRequest*>(event);
            std::cout << "Action request: " << actionRequest<< std::endl;
            break;
        }
        case UPNP_EVENT_RECEIVED: {
            const UpnpEvent* eventReceived = static_cast<const UpnpEvent*>(event);
            std::cout << "Event received: " << eventReceived << std::endl;
            break;
        }
        default:
            std::cout << "Other event received: " << eventType << std::endl;
            break;
    }
    return 0;
}


oatpp::String StaticFilesManager::getExtension(const oatpp::String& filename) {
  v_int32 dotPos = 0;
  for(v_int32 i = filename->size() - 1; i > 0; i--) {
    if(filename->data()[i] == '.') {
      dotPos = i;
      break;
    }
  }
  if(dotPos != 0 && dotPos < filename->size() - 1) {
    return oatpp::String((const char*)&filename->data()[dotPos + 1], filename->size() - dotPos - 1);
  }
  return nullptr;
}

oatpp::String StaticFilesManager::getFile(const oatpp::String& path) {
  if(!path) {
    return nullptr;
  }
  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
  auto& file = m_cache [path];
  if(file) {
    return file;
  }
  oatpp::String filename = m_basePath + "/" + path;
  file = oatpp::String::loadFromFile(filename->c_str());
  return file;
}

long long StaticFilesManager::getFileSize(const oatpp::String& filenamePath){
  if(!filenamePath){
    return -1;
  }

  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
  auto& file = m_cache [filenamePath];
  if(file) {
    return sizeof(file);
  }
  oatpp::String filename = m_basePath + "/" + filenamePath;
  file = oatpp::String::loadFromFile(filename->c_str());
  return sizeof(file);

}


oatpp::String StaticFilesManager::guessMimeType(const oatpp::String& filename) {
  auto extension = getExtension(filename);
  if(extension) {
    
    if(extension == "m3u8"){
      return "application/x-mpegURL";
    } else if(extension == "mp4"){
      return "video/mp4";
    } else if(extension == "ts"){
      return "video/MP2T";
    } else if(extension == "mp3"){
      return "audio/mp3";
    }
    
  }
  return nullptr;
}

oatpp::String formatText(const char* text, ...) {
  char buffer[4097];
  va_list args;
  va_start (args, text);
  vsnprintf(buffer, 4096, text, args);
  va_end(args);
  return oatpp::String(buffer);
}

v_int64 getMillisTickCount(){
  std::chrono::milliseconds millis = std::chrono::duration_cast<std::chrono::milliseconds>
  (std::chrono::system_clock::now().time_since_epoch());
  return millis.count();
}


void printDocs(IXML_Document *doc){
    std::cout << ixmlPrintDocument(doc) << std::endl;
}

//----------------------
int UPnPDeviceManager::sendUpnpAction(const char* actionUrl, const char* serviceType, IXML_Document* actionDoc) {
    IXML_Document* responseDoc = nullptr;
    int result = UpnpSendAction(handle, actionUrl, serviceType, nullptr, actionDoc, &responseDoc);
    sleep(3);
    if (result == UPNP_E_SUCCESS) {
        std::cout << "Action sent successfully." << std::endl;
        char* responseStr = ixmlPrintDocument(responseDoc);
        //std::cout << responseStr << std::endl;
        if (responseStr) {
            ixmlFreeDOMString(responseStr);
        }
        ixmlDocument_free(responseDoc);
    } else {

        std::cerr << "Failed to send action. Error code: " << result << std::endl;
        std::cerr << "Response DOC: "; 
        printDocs(responseDoc);
    }

    ixmlDocument_free(actionDoc);
    return result;
}


void UPnPDeviceManager::startDeviceDiscoveryByTarget(const std::string& deviceInfo){
  //pass in device managers known devices list


int ret = UpnpSearchAsync(handle,10,deviceInfo.c_str(), &known_devices);
}

void UPnPDeviceManager::advertiseDevice(){
  int ret =UpnpSendAdvertisement(dev_handle, 1800);
     if (ret != UPNP_E_SUCCESS) {
            std::cout << "Error initializing UPnP: " << UpnpGetErrorMessage(ret) << std::endl;
        }
      
}






//---------------

//AV TRANSPORT
// 100:52235/dmr/AVTransport1.xml
IXML_Document* UPnPDeviceManager::generatePlayBackActionDoc(const char *actionName, const char *serviceType, const std::map<std::string, std::string> &args){
  IXML_Document* doc = ixmlDocument_createDocument();
  const char* serviceURN = "urn:schemas-upnp-org:service:" + *serviceType + *":1";
  const char* action =  "u:" + *actionName;
  IXML_Element* actionTag = ixmlDocument_createElementNS(doc, serviceURN, action);
  ixmlElement_setAttribute(actionTag, "xmlns:u", "urn:upnp-org:serviceId:AVTransport");

  for (const auto&arg : args) {
          IXML_Element* argElement = ixmlDocument_createElement(doc, arg.first.c_str());
          IXML_Node* argTextNode = ixmlDocument_createTextNode(doc, arg.second.c_str());
          ixmlNode_appendChild(&argElement->n, argTextNode);
          ixmlNode_appendChild(&actionTag->n, &argElement->n);
      }
       
  return doc;
}



IXML_Document* createActionDocument(const char *actionName, const char *serviceType, const std::map<std::string, std::string> &args)
{
    IXML_Document* doc = ixmlDocument_createDocument();

    // Create action element
    std::string actionTag = "u:" + std::string(actionName);
    std::cout << "serviceType: " << serviceType << std::endl;
    IXML_Element* actionElement = ixmlDocument_createElement(doc, actionTag.c_str());
    ixmlElement_setAttribute(actionElement, "xmlns:u","urn:samsung.com:service:MainTVAgent2:1");
    IXML_Element* body = ixmlDocument_createElementNS(doc, "http://schemas.xmlsoap.org/soap/envelope/", "s:Body");

    ixmlNode_appendChild(&(doc->n), &(actionElement->n));


     for (const auto& arg : args) {
        IXML_Element* argElement = ixmlDocument_createElement(doc, arg.first.c_str());
        IXML_Node* argTextNode = ixmlDocument_createTextNode(doc, arg.second.c_str());
        ixmlNode_appendChild(&argElement->n, argTextNode);
        ixmlNode_appendChild(&actionElement->n, &argElement->n);
    }
    std::cout << "doc_node" << ixmlPrintDocument(doc);

    return doc;
}


IXML_Document* createSetAVTransportURIDoc(const std::map<std::string, std::string>& args) {
    IXML_Document* doc = ixmlDocument_createDocument();
    
    IXML_Element* action = ixmlDocument_createElementNS(doc, "urn:schemas-upnp-org:service:AVTransport:1", "u:SetAVTransportURI");
    int ret = ixmlElement_setAttribute(action, "xmlns:u", "urn:upnp-org:serviceId:AVTransport");
    if(ret != 0){
        std::cout << "operation failed:" << ret << std::endl;
    }
    ixmlNode_appendChild(&(doc->n), &(action->n));

    for (const auto&arg : args) {
        IXML_Element* argElement = ixmlDocument_createElement(doc, arg.first.c_str());
        IXML_Node* argTextNode = ixmlDocument_createTextNode(doc, arg.second.c_str());
        ixmlNode_appendChild(&argElement->n, argTextNode);
        ixmlNode_appendChild(&action->n, &argElement->n);
    }
    
    return doc;
}

IXML_Document* getMediaInfoDoc(const std::map<std::string, std::string>& args){
    IXML_Document* doc = ixmlDocument_createDocument();
    IXML_Element* action = ixmlDocument_createElementNS(doc, "urn:schemas-upnp-org:service:AVTransport:1", "u:GetMediaInfo");
    int ret = ixmlElement_setAttribute(action, "xmlns:u", "urn:upnp-org:serviceId:AVTransport");   
    if(ret != 0){
        std::cout << "operation failed:" << ret << std::endl;
    }
    ixmlNode_appendChild(&(doc->n), &(action->n));
     for (const auto&arg : args) {
        IXML_Element* argElement = ixmlDocument_createElement(doc, arg.first.c_str());
        IXML_Node* argTextNode = ixmlDocument_createTextNode(doc, arg.second.c_str());
        ixmlNode_appendChild(&argElement->n, argTextNode);
        ixmlNode_appendChild(&action->n, &argElement->n);
    }
    return doc;
}

IXML_Document* createPlayDoc( const std::map<std::string, std::string>& args){
    
    IXML_Document* doc = ixmlDocument_createDocument();
    IXML_Element* action = ixmlDocument_createElementNS(doc, "urn:schemas-upnp-org:service:AVTransport:1", "u:Play");
    int ret = ixmlElement_setAttribute(action, "xmlns:u", "urn:upnp-org:serviceId:AVTransport");
    if(ret != 0){
        std::cout << "operation failed:" << ret << std::endl;
    }
    ixmlNode_appendChild(&(doc->n), &(action->n));
    for (const auto&arg : args) {
        IXML_Element* argElement = ixmlDocument_createElement(doc, arg.first.c_str());
        IXML_Node* argTextNode = ixmlDocument_createTextNode(doc, arg.second.c_str());
        ixmlNode_appendChild(&argElement->n, argTextNode);
        ixmlNode_appendChild(&action->n, &argElement->n);
    }
    
    return doc;
}

int getContext(const char* filepath){
    AVFormatContext *fmt_ctx = NULL;
    const AVDictionaryEntry *tag = NULL;
    int ret;
      if ((ret = avformat_open_input(&fmt_ctx, filepath, NULL, NULL)))
        return ret;
 
    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }
 
    while ((tag = av_dict_iterate(fmt_ctx->metadata, tag)))
        printf("%s=%s\n", tag->key, tag->value);
 
    avformat_close_input(&fmt_ctx);
    return 1;


}

IXML_Document* createMetadataDocument(std::string fileuri, StaticFilesManager* fileMan) {
   getContext(fileuri.c_str());
   oatpp::network::Url parsedURI = oatpp::network::Url::Parser::parseUrl(fileuri);
   std::string filepath = parsedURI.path->c_str();

   IXML_Document* doc = ixmlDocument_createDocument();
    IXML_Element* didlLite = ixmlDocument_createElementNS(doc, "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/", "DIDL-Lite");
    ixmlElement_setAttribute(didlLite, "xmlns:dc", "http://purl.org/dc/elements/1.1/");
    ixmlElement_setAttribute(didlLite, "xmlns:upnp", "urn:schemas-upnp-org:metadata-1-0/upnp/");
    ixmlNode_appendChild(&(doc->n), &(didlLite->n));

    IXML_Element* item = ixmlDocument_createElement(doc, "item");
    ixmlElement_setAttribute(item, "id", "0");
    ixmlElement_setAttribute(item, "parentID", "0");
    ixmlElement_setAttribute(item, "restricted", "False");
    ixmlNode_appendChild(&(didlLite->n), &(item->n));

    IXML_Element* title = ixmlDocument_createElement(doc, "dc:title");
    IXML_Node* titleText = ixmlDocument_createTextNode(doc, "bunny");
    ixmlNode_appendChild(&title->n, titleText);
    ixmlNode_appendChild(&item->n, &title->n);

    IXML_Element* creator = ixmlDocument_createElement(doc, "dc:creator");
    IXML_Node* creatorText = ixmlDocument_createTextNode(doc, "Example Artist");
    ixmlNode_appendChild(&creator->n, creatorText);
    ixmlNode_appendChild(&item->n, &creator->n);

    IXML_Element* res = ixmlDocument_createElement(doc, "res");
    ixmlElement_setAttribute(res, "protocolInfo", 
    "http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L3L_SD_AAC;DLNA.ORG_FLAGS=ED100000000000000000000000000000");
    ixmlElement_setAttribute(res, "sampleFrequency", "44100");
    //ixmlElement_setAttribute(res, "duration", "1:48:55.701");
    ixmlElement_setAttribute(res, "bitrate", "664683");
    IXML_Node* resText = ixmlDocument_createTextNode(doc, "");
    ixmlNode_appendChild(&res->n, resText);
    ixmlNode_appendChild(&item->n, &res->n);


    
    IXML_Element* upnpClass = ixmlDocument_createElement(doc, "upnp:class");
    IXML_Node* classText = ixmlDocument_createTextNode(doc, "object.item.videoItem");
    ixmlNode_appendChild(&upnpClass->n, classText);
    ixmlNode_appendChild(&item->n, &upnpClass->n);
    
    return doc;


}

IXML_Document* createMetadataDocument() {
    IXML_Document* doc = ixmlDocument_createDocument();
    IXML_Element* didlLite = ixmlDocument_createElementNS(doc, "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/", "DIDL-Lite");
    ixmlElement_setAttribute(didlLite, "xmlns:dc", "http://purl.org/dc/elements/1.1/");
    ixmlElement_setAttribute(didlLite, "xmlns:upnp", "urn:schemas-upnp-org:metadata-1-0/upnp/");
    ixmlNode_appendChild(&(doc->n), &(didlLite->n));

    IXML_Element* item = ixmlDocument_createElement(doc, "item");
    ixmlElement_setAttribute(item, "id", "0");
    ixmlElement_setAttribute(item, "parentID", "0");
    ixmlElement_setAttribute(item, "restricted", "False");
    ixmlNode_appendChild(&(didlLite->n), &(item->n));

    IXML_Element* title = ixmlDocument_createElement(doc, "dc:title");
    IXML_Node* titleText = ixmlDocument_createTextNode(doc, "bunny");
    ixmlNode_appendChild(&title->n, titleText);
    ixmlNode_appendChild(&item->n, &title->n);

    IXML_Element* creator = ixmlDocument_createElement(doc, "dc:creator");
    IXML_Node* creatorText = ixmlDocument_createTextNode(doc, "Example Artist");
    ixmlNode_appendChild(&creator->n, creatorText);
    ixmlNode_appendChild(&item->n, &creator->n);

/*
<res nrAudioChannels="2" duration="1:48:55.701" bitrate="327040"
microsoft:codec="{34363248-0000-0010-8000-00AA00389B71}"
protocolInfo="http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L3L_SD_AAC;DLNA.ORG_OP=10;DLNA.ORG_CI=1;DLNA.ORG_FLAGS=01700000000000000000000000000000"
sampleFrequency="44100" resolution="640x360">
"http://192.168.0.212:10246/MDEServer/89A83FB7-F5E2-4BE6-B5AC-0676CCB2C3F5/1000.mp4?formatID=00000041-A9AF-4584-84E2-55BFEF0A7D7E,instance=1"
   </res>
   */

  /*
 res bitrate="327040" resolution="640x360"
            protocolInfo="http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L3L_SD_AAC;DLNA.ORG_CI=1;DLNA.ORG_FLAGS=01700000000000000000000000000000"
            sampleFrequency="44100" nrAudioChannels="2"
            microsoft:codec="{34363248-0000-0010-8000-00AA00389B71}">
            http://192.168.0.212:10246/MDEServer/9BC98514-6D84-47B9-9E7B-A1E70D6F7E65/1000.mp4?formatID=00000041-A9AF-4584-84E2-55BFEF0A7D7E</res> 
  */
    IXML_Element* res = ixmlDocument_createElement(doc, "res");
    ixmlElement_setAttribute(res, "protocolInfo", 
    "http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L3L_SD_AAC;DLNA.ORG_FLAGS=ED100000000000000000000000000000");
    ixmlElement_setAttribute(res, "sampleFrequency", "44100");
    ixmlElement_setAttribute(res, "duration", "00:01:00.701");
    ixmlElement_setAttribute(res, "bitrate", "664683");
    ixmlElement_setAttribute(res, "resolution","640x360");
    IXML_Node* resText = ixmlDocument_createTextNode(doc, "http://192.168.0.212:8000/media/video/big_buck_bunny.mp4");
    ixmlNode_appendChild(&res->n, resText);
    ixmlNode_appendChild(&item->n, &res->n);


    
    IXML_Element* upnpClass = ixmlDocument_createElement(doc, "upnp:class");
    IXML_Node* classText = ixmlDocument_createTextNode(doc, "object.item.videoItem");
    ixmlNode_appendChild(&upnpClass->n, classText);
    ixmlNode_appendChild(&item->n, &upnpClass->n);
    return doc;
}






