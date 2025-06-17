#pragma once

#include "../../../../core/sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Module_os {
public:
    SK_Global* skg;

    SK_Module_os(SK_Global* _skg) {
        skg = _skg;
    }
    
    ~SK_Module_os(){
        skg = nullptr;
    }

    void handleOperation(const SK_String& operation, nlohmann::json& payload, SK_Communication_Response& respondWith) {
             if (operation == "getCPUInfo"    ) getCPUInfo(respondWith);
        else if (operation == "getMemoryInfo" ) getMemoryInfo(respondWith);
        else if (operation == "getMachineTime") getMachineTime(respondWith);
        else if (operation == "getNetworInfo" ) getNetworInfo(respondWith);
        else if (operation == "getUserInfo"   ) getUserInfo(respondWith);
    };



    
    void getCPUInfo(SK_Communication_Response& respondWith) {
        respondWith.JSON(static_cast<SK_Machine*>(skg->machine)->getCPUInfo());
    };

    void getMemoryInfo(SK_Communication_Response& respondWith) {
        respondWith.JSON(static_cast<SK_Machine*>(skg->machine)->getMemoryInfo());
    };

    void getMachineTime(SK_Communication_Response& respondWith) {
        respondWith.JSON(static_cast<SK_Machine*>(skg->machine)->getMachineType());
    };

    void getNetworInfo(SK_Communication_Response& respondWith) {
        respondWith.JSON(static_cast<SK_Machine*>(skg->machine)->getNetworkInfo());
    };

    void getUserInfo(SK_Communication_Response& respondWith) {
        respondWith.JSON(static_cast<SK_Machine*>(skg->machine)->getUserInfo());
    };
};

END_SK_NAMESPACE
