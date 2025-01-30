#pragma once

#include "../../../../core/sk_common.hxx"

BEGIN_SK_NAMESPACE

class SK_Module_os {
public:
    void handleOperation(const SK_String& operation, const SK_JSON_YY& payload, SK_Communication_Response& respondWith) {
             if (operation == "getCPUInfo"    ) getCPUInfo(respondWith);
        else if (operation == "getMemoryInfo" ) getMemoryInfo(respondWith);
        else if (operation == "getMachineTime") getMachineTime(respondWith);
        else if (operation == "getNetworInfo" ) getNetworInfo(respondWith);
        else if (operation == "getUserInfo"   ) getUserInfo(respondWith);
    };



    
    void getCPUInfo(SK_Communication_Response& respondWith) {
        respondWith.JSON(SK_Machine::getCPUInfo());
    };

    void getMemoryInfo(SK_Communication_Response& respondWith) {
        respondWith.JSON(SK_Machine::getMemoryInfo());
    };

    void getMachineTime(SK_Communication_Response& respondWith) {
        respondWith.JSON(SK_Machine::getMachineType());
    };

    void getNetworInfo(SK_Communication_Response& respondWith) {
        respondWith.JSON(SK_Machine::getNetworkInfo());
    };

    void getUserInfo(SK_Communication_Response& respondWith) {
        respondWith.JSON(SK_Machine::getUserInfo());
    };
};

END_SK_NAMESPACE