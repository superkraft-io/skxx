#pragma once

#include "../../../../../core/sk_common.hpp"

#if defined(SK_OS_windows)
    #include "shellapi.h"
#elif defined(SK_OS_apple)
    //...
#elif defined(SK_OS_linux)
    //...
#endif

BEGIN_SK_NAMESPACE

class SK_Module_ProtonJS_Shell {
public:
    SK_Global* skg;

    SK_Module_ProtonJS_Shell(SK_Global* _skg) {
        skg = _skg;
    }
    
    ~SK_Module_ProtonJS_Shell(){
        skg = nullptr;
    }

    void handleOperation(const SK_String& operation, nlohmann::json& payload, SK_Communication_Response& respondWith) {
        
        SK_String target = payload["__moduleInstanceConfig"]["__target"];
        
            if (operation == "showItemInFolder") showItemInFolder(payload, respondWith);
       else if (operation == "openPath") openPath(payload, respondWith);
       else if (operation == "openExternal") openExternal(payload, respondWith);
       else if (operation == "trashItem") trashItem(payload, respondWith);
       else if (operation == "beep") beep(payload, respondWith);
       else if (operation == "writeShortcutLink") writeShortcutLink(payload, respondWith);
       else if (operation == "readShortcutLink") readShortcutLink(payload, respondWith);
       
    };

    void showItemInFolder(const nlohmann::json& payload, SK_Communication_Response& respondWith) {

        #if defined(SK_OS_windows)
            //...
        #elif defined(SK_OS_apple)
            //...
        #elif defined(SK_OS_linux)
            //...
        #endif

        respondWith.JSON_OK();
    }

    void openPath(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        SK_String path = payload["path"];

        #if defined(SK_OS_windows)
            ShellExecuteW(0, 0, path.toWString().c_str(), 0, 0, SW_SHOW);
        #elif defined(SK_OS_apple)
            //...
        #elif defined(SK_OS_linux)
            //...
        #endif

        respondWith.JSON_OK();
    }

    void openExternal(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        SK_String url = payload["url"];

        #if defined(SK_OS_windows)
            ShellExecuteW(0, 0, url.toWString().c_str(), 0, 0, SW_SHOW);
        #elif defined(SK_OS_apple)
            //...
        #elif defined(SK_OS_linux)
            //...
        #endif

        respondWith.JSON_OK();
    }

    void trashItem(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        #if defined(SK_OS_windows)
            //...
        #elif defined(SK_OS_apple)
            //...
        #elif defined(SK_OS_linux)
            //...
        #endif
        
        respondWith.JSON_OK();
    }

    void beep(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        #if defined(SK_OS_windows)
            //...
        #elif defined(SK_OS_apple)
            //...
        #elif defined(SK_OS_linux)
            //...
        #endif
        
        respondWith.JSON_OK();
    }

    void writeShortcutLink(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        #if defined(SK_OS_windows)
            //...
        #elif defined(SK_OS_apple)
            //...
        #elif defined(SK_OS_linux)
            //...
        #endif
        
        respondWith.JSON_OK();
    }

    void readShortcutLink(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        #if defined(SK_OS_windows)
            //...
        #elif defined(SK_OS_apple)
            //...
        #elif defined(SK_OS_linux)
            //...
        #endif
        
        respondWith.JSON_OK();
    }

    
};

END_SK_NAMESPACE
