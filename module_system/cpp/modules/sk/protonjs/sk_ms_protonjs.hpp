#pragma once

#include "../../../../../core/sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Module_ProtonJS {
public:
    SK_Global* skg;

    SK_Module_ProtonJS_App* app;
    SK_Module_ProtonJS_Window* window;
    SK_Module_ProtonJS_Shell* shell;
    
    SK_Module_ProtonJS(SK_Global* _skg) {
        skg = _skg;

        app    = new SK_Module_ProtonJS_App(_skg);
        window = new SK_Module_ProtonJS_Window(_skg);
        shell  = new SK_Module_ProtonJS_Shell(_skg);
    }

    ~SK_Module_ProtonJS(){
        delete app;
        delete window;
        delete shell;
        
        skg = nullptr;
    }
    
    
    void handleOperation(const SK_String& operation, nlohmann::json& payload, SK_Communication_Response& respondWith) {
        
        SK_String target = payload["__moduleInstanceConfig"]["__target"];
        
             if (target == "app") app->handleOperation(operation, payload, respondWith);
        else if (target == "window") window->handleOperation(operation, payload, respondWith);
        else if (target == "shell") shell->handleOperation(operation, payload, respondWith);
    };
};

END_SK_NAMESPACE
