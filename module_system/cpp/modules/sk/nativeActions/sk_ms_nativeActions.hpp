#pragma once

#include "../../../../../core/sk_common.hpp"
#include "../../../../../../sk_app_nativeActions/sk_app_nativeActions.hpp"

#include "core/sk_ms_cNA_handlePluginParamMouseEvent.hpp"

BEGIN_SK_NAMESPACE

class SK_Module_NativeActions {
public:
    SK_Global* skg;

    SK_MS_cNA_handlePluginParamMouseEvent handlePluginParamMouseEvent;
    
    SK_App_NativeActions* appActions;

    SK_Module_NativeActions(SK_Global* _skg) {
        skg = _skg;

        appActions = new SK_App_NativeActions();
    }

    void handleOperation(const SK_String& operation, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        if (appActions->handleOperation(operation, payload, respondWith)) return;

        if (operation == "handlePluginParamMouseEvent") handlePluginParamMouseEvent.handleOperation(payload, respondWith);

    };

    SK_String listActions() {
        SK_String keysString;

        for (const auto& pair : appActions->actions) {
            if (!keysString.data.empty()) {
                keysString += ",";
            }
            keysString += "'" + pair.first + "'";;
        }

        if (keysString.length() > 0) keysString += ",";

        keysString = "[" + keysString;
        keysString += "'handlePluginParamMouseEvent']";

        return keysString;
    }
};

END_SK_NAMESPACE
