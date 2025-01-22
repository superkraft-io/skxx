#pragma once

#include "../../../../core/sk_common.hxx"

BEGIN_SK_NAMESPACE

class SK_Module_application {
public:
    void SK_Module_application::handleOperation(const SK_String& operation, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        if (operation == "getStaticInfo") getStaticInfo(respondWith);
    };

    void SK_Module_application::getStaticInfo(SK_Communication_Response& respondWith) {
        respondWith.JSON(SK_Machine::getStaticInfo());
    };
};

END_SK_NAMESPACE