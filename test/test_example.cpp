#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/core/base/Environment.hpp"
#include <oatpp/network/Server.hpp>



#include <unistd.h>
#include "upnp/upnp.h"
#include "upnp/UpnpString.h"
#include "upnp/UpnpDiscovery.h" 
#include "upnp/upnpdebug.h"
#include <iostream>
#include "upnp/upnptools.h"
#include "upnp/ixml.h"
#include <netdb.h>
#include <map>


void printDocs(IXML_Document *doc){
    std::cout << ixmlPrintDocument(doc) << std::endl;
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
    IXML_Node* titleText = ixmlDocument_createTextNode(doc, "BisexualLight");
    ixmlNode_appendChild(&title->n, titleText);
    ixmlNode_appendChild(&item->n, &title->n);

    IXML_Element* creator = ixmlDocument_createElement(doc, "dc:creator");
    IXML_Node* creatorText = ixmlDocument_createTextNode(doc, "Example Artist");
    ixmlNode_appendChild(&creator->n, creatorText);
    ixmlNode_appendChild(&item->n, &creator->n);



    IXML_Element* res = ixmlDocument_createElement(doc, "res");
    ixmlElement_setAttribute(res, "protocolInfo", "http-get:*:audio/mpeg:DLNA.ORG_OP=01;DLNA.ORG_CI=0;DLNA.ORG_FLAGS=01700000000000000000000000000000");
    IXML_Node* resText = ixmlDocument_createTextNode(doc, "http://192.168.0.212:8000/media/audio/BisexualLight.mp3");
    ixmlNode_appendChild(&res->n, resText);
    ixmlNode_appendChild(&item->n, &res->n);


    
    IXML_Element* upnpClass = ixmlDocument_createElement(doc, "upnp:class");
    IXML_Node* classText = ixmlDocument_createTextNode(doc, "object.item.audioItem");
    ixmlNode_appendChild(&upnpClass->n, classText);
    ixmlNode_appendChild(&item->n, &upnpClass->n);
    return doc;
}

int clientCallback(Upnp_EventType eventType, const void* event, void* cookie) {
    std::cout << "EventType: " << eventType << std::endl;
    switch (eventType) {
        case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE: {}
        case UPNP_DISCOVERY_SEARCH_RESULT: {
            const UpnpDiscovery* discoveryEvent = static_cast<const UpnpDiscovery*>(event);
            const char *deviceId = UpnpDiscovery_get_DeviceID_cstr(discoveryEvent);
            const char *deviceType = UpnpDiscovery_get_DeviceType_cstr(discoveryEvent);
            const char *location = UpnpDiscovery_get_Location_cstr(discoveryEvent);
            const char * date = UpnpDiscovery_get_Date_cstr(discoveryEvent);
            std::cout << "Search result: " << std::endl;
            std::cout << deviceId << std::endl;
            std::cout << location << std::endl;
            std::cout << deviceType << std::endl;
            break;
        }
        default:
            std::cout << "Other event received: " << eventType << std::endl;
            break;
    }
    return 1;

}

int upnpCallback(Upnp_EventType eventType, const void* event, void* cookie) {
    switch (eventType) {
        case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE: {
            
        }
        case UPNP_DISCOVERY_SEARCH_RESULT: {
            const UpnpDiscovery* discoveryEvent = static_cast<const UpnpDiscovery*>(event);
            const char *deviceId = UpnpDiscovery_get_DeviceID_cstr(discoveryEvent);
            const char *deviceType = UpnpDiscovery_get_DeviceType_cstr(discoveryEvent);
            const char *location = UpnpDiscovery_get_Location_cstr(discoveryEvent);
            const char * date = UpnpDiscovery_get_Date_cstr(discoveryEvent);
            const sockaddr_storage *peerAddr = UpnpDiscovery_get_DestAddr(discoveryEvent);
            socklen_t peer_addrlen;
            char host[NI_MAXHOST];

            ssize_t amount;
            char buffer[1000];
            int info_code = getnameinfo((struct sockaddr *)&peerAddr, peer_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
            std::cout << "Search result: " << std::endl;
           
        }
        
        case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE: {
            const UpnpDiscovery* discoveryEvent = static_cast<const UpnpDiscovery*>(event);
            std::cout << "Device removed: " << discoveryEvent << std::endl;
            break;
        }
        case UPNP_EVENT_SUBSCRIPTION_REQUEST: {
            const UpnpSubscriptionRequest* subscriptionRequest = static_cast<const UpnpSubscriptionRequest*>(event);
            std::cout << "Subscription request from: " << subscriptionRequest << std::endl;
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
class Upnp_Actions {
    std::string action;
    std::string device;
};


IXML_Document* createActionDocument(const char * actionName, const char* serviceType, const std::map<std::string, std::string>& args){
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


IXML_Document* GetTransportInfo( const std::map<std::string, std::string>& args){
    IXML_Document* doc = ixmlDocument_createDocument();
    IXML_Element* action = ixmlDocument_createElementNS(doc, "urn:schemas-upnp-org:service:AVTransport:1", "u:GetTransportInfo");
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

IXML_Document* RunBrowser( const std::map<std::string, std::string>& args){
    IXML_Document* doc = ixmlDocument_createDocument();
    IXML_Element* action = ixmlDocument_createElementNS(doc, "urn:samsung.com:service:MainTVAgent2:1", "u:RunBrowser");
    int ret = ixmlElement_setAttribute(action, "xmlns:u", "urn:upnp-org:serviceId:AVTransport");
    ixmlNode_appendChild(&(doc->n), &(action->n));

    for (const auto&arg : args) {
        IXML_Element* argElement = ixmlDocument_createElement(doc, arg.first.c_str());
        IXML_Node* argTextNode = ixmlDocument_createTextNode(doc, arg.second.c_str());
        ixmlNode_appendChild(&argElement->n, argTextNode);
        ixmlNode_appendChild(&action->n, &argElement->n);
    }
    return doc;
}

int sendUpnpAction(UpnpClient_Handle clientHandle, const char* actionUrl, const char* serviceType, IXML_Document* actionDoc) {
    IXML_Document* responseDoc = nullptr;
    int result = UpnpSendAction(clientHandle, actionUrl, serviceType, nullptr, actionDoc, &responseDoc);

    if (result == UPNP_E_SUCCESS) {
        std::cout << "Action sent successfully." << std::endl;
        char* responseStr = ixmlPrintDocument(responseDoc);
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




#include OATPP_CODEGEN_END(ApiController) ///< End ApiController codegen section


void sendActions(){
    /////////////////////////////////////
    UpnpClient_Handle cl_handle;

    UpnpRegisterClient(clientCallback, nullptr, &cl_handle);

    const char* actionUrl = "http://192.168.0.100:52235/upnp/control/AVTransport1";
    const char* serviceType = "urn:schemas-upnp-org:service:AVTransport:1";
  //  

    std::map<std::string, std::string> args = {
        {"InstanceID","0000"},
        {"CurrentURI","http://192.168.0.212:8000/BisexualLight.mp3"},
        {"CurrentURIMetaData", ixmlPrintDocument(createMetadataDocument())}
    };


    IXML_Document* setAvTransportURIDoc = createSetAVTransportURIDoc(args);

    printDocs(setAvTransportURIDoc);

    int ret =sendUpnpAction(cl_handle, actionUrl, serviceType, setAvTransportURIDoc);

  // 
////////////////////
/*
  actionUrl = "http://192.168.0.100:52235/upnp/control/AVTransport1";
    serviceType = "urn:schemas-upnp-org:service:AVTransport:1";
    args = {
        {"InstanceID","0000"},
        };
    IXML_Document* transport_doc = GetTransportInfo(args);
    std::cout << "transport doc to be sent: -------------------------------" << std::endl;
    printDocs(transport_doc);
    sendUpnpAction(cl_handle, actionUrl, serviceType, transport_doc);
  //  //////////////////////////////////////////////////
    actionUrl = "http://192.168.0.100:52235/upnp/control/AVTransport1";
    serviceType = "urn:schemas-upnp-org:service:AVTransport:1";
    args = {
        {"InstanceID","0000"},
        {"Speed","1"},
        };
    IXML_Document* play_media = createPlayDoc(args);
    std::cout << "play media doc to be sent: -------------------------------" << std::endl;
    printDocs(play_media);

    sendUpnpAction(cl_handle, actionUrl, serviceType, play_media);
    */
    ///////////////////////////////////////////////////
    
    actionUrl = "http://192.168.0.100:52235/MainTVServer2/control/MainTVAgent2";
    serviceType = "urn:samsung.com:service:MainTVAgent2:1";
    args = {
        {"BrowserURL","http://192.168.0.212:8000/BisexualLight.mp3"},
        };
    IXML_Document* browser_doc = RunBrowser(args);
    std::cout << "Run Browser doc to be sent: -------------------------------" << std::endl;
    printDocs(browser_doc);
    sendUpnpAction(cl_handle, actionUrl, serviceType, browser_doc);

   
}

class Handler : public oatpp::web::server::HttpRequestHandler {
public:

  /**
   * Handle incoming request and return outgoing response.
   */
  std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
    sendActions();
    return ResponseFactory::createResponse(Status::CODE_200, "Hello World!");
  }
};




int main() {

    
int ret = UpnpInit2(nullptr, 8001);
if (ret != UPNP_E_SUCCESS) {
    std::cerr << "UPnP initialization failed with value"<< ret << std::endl;
    return EXIT_FAILURE;
}
    sendActions();
    while (true)
    {
        
    }
    
    UpnpFinish();
    return EXIT_SUCCESS;
}

