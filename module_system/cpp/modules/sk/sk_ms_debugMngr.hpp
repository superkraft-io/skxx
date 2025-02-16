#pragma once

#include "../../../../core/sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Module_debugMngr {
public:
    void handleOperation(const SK_String& operation, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
             if (operation == "showDevTools") showDevTools(payload, respondWith);
    };

    void showDevTools(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        SK_String target = payload["target"];

        if (target == "sb") {
            SK_Global::showSoftBackendDevTools();
            return;
        }

        SK_Window* wnd = SK_Global::onFindWindowByTag(target);

        if (wnd == nullptr) {
            respondWith.error(404, "ENOENT");
            return;
        }

        //wnd->webview.showDevTools();

        
        respondWith.JSON_OK();
    };
};

END_SK_NAMESPACE
