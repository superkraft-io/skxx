#pragma once

#include "../../../../../core/sk_common.hxx"
#include "../../../../../../sk_app_nativeActions/sk_app_nativeActions.hxx"

#include "core/sk_ms_cNA_handleParamMouseEvent.hxx"

BEGIN_SK_NAMESPACE

class SK_Module_NativeActions {
public:
    SK_MS_cNA_handleParamComponentMouseEvent handleParamComponentMouseEvent;
    
    SK_App_NativeActions appActions;

    void handleOperation(const SK_String& operation, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        if (appActions.handleOperation(operation, payload, respondWith)) return;

        if (operation == "handleParamComponentMouseEvent") handleParamComponentMouseEvent.handleOperation(payload, respondWith);

    };

    SK_String listActions() {
        SK_String keysString;

        for (const auto& pair : appActions.actions) {
            if (!keysString.data.empty()) {
                keysString += ",";
            }
            keysString += "'" + pair.first + "'";;
        }

        if (keysString.length() > 0) keysString += ",";

        keysString = "[" + keysString;
        keysString += "'handleParamComponentMouseEvent']";

        return keysString;
    }
};

END_SK_NAMESPACE
