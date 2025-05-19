#pragma once

#include "../../../../../core/sk_common.hpp"


//references:

//Macos
//https://github.com/electron/electron/blob/530ccfe350498d8bdaa26d96e4b4d81df4098848/shell/browser/browser.cc#L86
//https://github.com/electron/electron/blob/530ccfe350498d8bdaa26d96e4b4d81df4098848/shell/browser/browser_mac.mm

//Windows
//https://github.com/electron/electron/blob/530ccfe350498d8bdaa26d96e4b4d81df4098848/shell/browser/browser_win.cc

BEGIN_SK_NAMESPACE

class SK_Module_ProtonJS_App {
public:
    SK_Global* skg;

    SK_Window_Mngr* wndMngr;
    
    SK_Module_ProtonJS_App(SK_Global* _skg) {
        skg = _skg;
    }
    
    ~SK_Module_ProtonJS_App(){
        wndMngr = nullptr;
        skg = nullptr;
    }

    void handleOperation(const SK_String& operation, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        
        SK_String target = payload["__moduleInstanceConfig"]["__target"];
        
             if (operation == "quit") quit(payload, respondWith);
       else if (operation == "getSystemLocale") getSystemLocale(payload, respondWith);
       else if (operation == "haveAllWindowsClosed") haveAllWindowsClosed(payload, respondWith);
    };

    void quit(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        

        respondWith.JSON_OK();
    }

    void getSystemLocale(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        SK_String sysLocale;
        
        #if defined(SK_OS_windows)
            //for linux
        #elif defined(SK_OS_apple)
            #ifdef __OBJC__
                NSLocale *currentLocale = [NSLocale currentLocale];
                sysLocale = [currentLocale localeIdentifier];
            #endif
        #elif defined(SK_OS_windows)
            //for linux
        #endif
        
        respondWith.JSON({
            {"sysLocale", sysLocale}
        });
    }
    
    void haveAllWindowsClosed(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        size_t total = wndMngr->list.size();
        respondWith.JSON({{"allClosed", false}});
    }
};

END_SK_NAMESPACE
