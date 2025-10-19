#pragma once

#include "../../../../core/sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Module_debugMngr {
public:
    SK_Global* skg;

    SK_Module_debugMngr(SK_Global* _skg) {
        skg = _skg;
    }
    
    ~SK_Module_debugMngr() {
        skg = nullptr;
    }

    void handleOperation(const SK_String& operation, nlohmann::json& payload, SK_Communication_Response& respondWith) {
             if (operation == "enable") enable(payload, respondWith);
        else if (operation == "showDevTools") showDevTools(payload, respondWith);
    };

    void enable(nlohmann::json& payload, SK_Communication_Response& respondWith) {
        bool enable = false;
        if (payload.contains("enable")) enable = payload["enable"];
       
        if (skg->enableDebug_Views) skg->enableDebug_Views(enable);

        respondWith.JSON_OK();
    }


    void showDevTools(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        SK_String target = payload["target"];

        if (target == "sb") {
            skg->showSoftBackendDevTools();
            return;
        }

        SK_Window* wnd = skg->onFindWindowByTag(target);

        if (wnd == nullptr) {
            respondWith.error(404, "ENOENT");
            return;
        }

        wnd->webview.showDevTools();
        
        respondWith.JSON_OK();
    };

    
};

END_SK_NAMESPACE
