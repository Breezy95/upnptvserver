#ifndef Utils_hpp
#define Utils_hpp

#include "oatpp/core/concurrency/SpinLock.hpp"


#include <unordered_map>
#include <set>
#include <map>
#include <string>
#include "oatpp/core/Types.hpp"
#include "upnp/upnp.h"
#include "upnp/upnptools.h"
#include "upnp/UpnpDiscovery.h"
#include <iostream>

// FILE STREAMING STUFF
///////////////////////////////////

extern "C" {
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
#include <gupnp-av-1.0/libgupnp-av/gupnp-av.h>
#include <gupnp-dlna-2.0/libgupnp-dlna/gupnp-dlna-profile-guesser.h>

static inline const char* ts_make_string(std::string buf, int64_t ts)
{

    if (ts == AV_NOPTS_VALUE) snprintf(&buf[0], AV_TS_MAX_STRING_SIZE, "NOPTS");
    else                      snprintf(&buf[0], AV_TS_MAX_STRING_SIZE, "%" PRId64, ts);
    return buf.c_str();
}

static inline const char *ts_make_time_string(std::string buf, int64_t ts, AVRational *tb)
{
    if (ts == AV_NOPTS_VALUE) snprintf(&buf[0], AV_TS_MAX_STRING_SIZE, "NOPTS");
    else                      snprintf(&buf[0], AV_TS_MAX_STRING_SIZE, "%.6g", av_q2d(*tb) * ts);
    return buf.c_str();
} 
static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{


    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
    std::string s;
 
    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           ts_make_string(s.data(), pkt->pts), ts_make_time_string(s.data(), pkt->pts, time_base),
           ts_make_string(s.data(), pkt->dts), ts_make_time_string(s.data(), pkt->dts, time_base),
           ts_make_string(s.data(), pkt->duration), ts_make_time_string(s.data(), pkt->duration, time_base),
           pkt->stream_index);
}


extern const char* base_desc;

struct deviceInfo {
	std::string id,type,location;
};


int upnpServerCallback(Upnp_EventType eventType, const void* event, void* cookie);

int upnpClientCallback(Upnp_EventType eventType, const void* event, void* cookie);


class StaticFilesManager {
private:
  oatpp::String m_basePath;
  oatpp::concurrency::SpinLock m_lock;
  std::unordered_map<oatpp::String, oatpp::String> m_cache;
private:
 
public:
   oatpp::String getExtension(const oatpp::String& filename);
  StaticFilesManager(const oatpp::String& basePath)
    : m_basePath(basePath)
  {}

  oatpp::String getFile(const oatpp::String& path);
  oatpp::String guessMimeType(const oatpp::String& filename);
  long long getFileSize(const oatpp::String& filename);
  oatpp::String getFileChunk(const oatpp::String& filepath, std::string lowerRange, std::string upperRange);  
};

oatpp::String formatText(const char* text, ...);
v_int64 getMillisTickCount();




IXML_Document* createActionDocument(const char * actionName, const char* serviceType, const std::map<std::string, std::string>& args);
//IXML_Document* createMetadataDocument();
IXML_Document* createMetadataDocument(std::string fileuri, std::shared_ptr<StaticFilesManager> fileMan);
IXML_Document* createMetadataDocument();
int sendUpnpAction(UpnpClient_Handle clientHandle, const char* actionUrl, const char* serviceType, IXML_Document* actionDoc);
void printDocs(IXML_Document *doc);
IXML_Document* createSetAVTransportURIDoc(const std::map<std::string, std::string>& args);
IXML_Document* GetTransportInfo( const std::map<std::string, std::string>& args);
IXML_Document* createPlayDoc( const std::map<std::string, std::string>& args);



class UPnPDeviceManager {
public:
std::vector<deviceInfo> known_devices;
    UPnPDeviceManager() {
        int ret = UpnpInit2(nullptr, 0);
        if (ret != UPNP_E_SUCCESS) {
            std::cout << "Error initializing UPnP: " << UpnpGetErrorMessage(ret) << std::endl;
        }

        ret =UpnpRegisterClient(upnpClientCallback,nullptr,&handle);
        if (ret != UPNP_E_SUCCESS) {
            std::cout << "Error initializing control point: " << UpnpGetErrorMessage(ret) << std::endl;
        }

        ret = UpnpSetWebServerRootDir("/");
        if (ret != UPNP_E_SUCCESS) {
            std::cout << "Error setting webdirectory: " << UpnpGetErrorMessage(ret) << std::endl;
        }

        std::cout << "IP ADDRESS: " << UpnpGetServerIpAddress() << std::endl;
        ret =UpnpRegisterRootDevice2(UPNPREG_BUF_DESC,base_desc,1076,1,upnpServerCallback,nullptr,&dev_handle);
        if (ret != UPNP_E_SUCCESS) {
            std::cout << "Error initializing device: " << UpnpGetErrorMessage(ret) << std::endl;
        }


    if(ret != 0){
        std::cout << "operation failed:" << ret << std::endl;
    }
    }

    ~UPnPDeviceManager() {
        UpnpFinish();
    }

   int sendUpnpAction(const char* actionUrl, const char* serviceType, IXML_Document* actionDoc);
   void discoverDevices();
   void advertiseDevice();
   void startDeviceDiscoveryByTarget(const std::string& deviceInfo);
   IXML_Document* generatePlayBackActionDoc(const char *actionName, const char *serviceType, const std::map<std::string, std::string> &args);
    

private:
    UpnpClient_Handle handle;
    UpnpDevice_Handle dev_handle;
};





#endif /* Utils_hpp */