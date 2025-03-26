#pragma once

#include "../../../../core/sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Module_application {
public:
    SK_Global* skg;

    SK_Module_application(SK_Global* _skg) {
        skg = _skg;
    }

    void handleOperation(const SK_String& operation, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        if (operation == "getStaticInfo") getStaticInfo(respondWith);
    };

    void getStaticInfo(SK_Communication_Response& respondWith) {
        respondWith.JSON(static_cast<SK_Machine*>(skg->machine)->getStaticInfo());
    };
};

END_SK_NAMESPACE
