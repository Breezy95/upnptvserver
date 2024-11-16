#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include "../file_server/Utils.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)


class deviceInfoDTO :public oatpp::DTO {
  DTO_INIT(deviceInfoDTO, DTO)

  DTO_FIELD(String, ID, "deviceId");
  DTO_FIELD(String, type, "deviceType");
  DTO_FIELD(String, location, "deviceIP");
};

class KVPairDTO : public oatpp::DTO {
   DTO_INIT(KVPairDTO, DTO)

    DTO_FIELD(oatpp::String, key);   
    DTO_FIELD(oatpp::String, value); 
};


class AVTransportDTO : public oatpp::DTO {
  DTO_INIT(AVTransportDTO, DTO)

  DTO_FIELD(oatpp::Object<deviceInfoDTO>, device);
  DTO_FIELD(List<oatpp::Object<KVPairDTO>>, argList);
  DTO_FIELD(String, action, "AVaction");
  DTO_FIELD(String, instanceID, "AVInstanceID") = "0000";
  DTO_FIELD(String, filepath, "AVfilepath");
  DTO_FIELD(String, servicetype, "AVserviceType");
  DTO_FIELD(String, currentURI, "AVCurrentURI");
  DTO_FIELD(String, currentURIMetaData, "AVCurrentURIMetadata");
};

#include OATPP_CODEGEN_END(DTO)
