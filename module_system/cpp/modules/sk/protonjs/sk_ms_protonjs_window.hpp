#pragma once

#include "../../../../../core/sk_common.hpp"

#ifdef __OBJC__
    #import <AppKit/AppKit.h>
#endif

BEGIN_SK_NAMESPACE

class SK_Module_ProtonJS_Window {
public:
    SK_Window_Mngr* wndMngr;
    
    
    
    void handleOperation(const SK_String& operation, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        
        SK_String wndID = payload["__moduleInstanceConfig"]["__uuid"];
        SK_Window* wnd = wndMngr->findWindowByTag(wndID);
        
              if (operation == "construct") construct(wnd, payload, respondWith);
         else if (operation == "configure") configure(wnd, payload, respondWith);
         else if (operation == "loadURL") loadURL(wnd, payload, respondWith);
    };

    void construct(SK_Window* _wnd, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        
        if (_wnd != nullptr) return;
        
        SK_String wndID = payload["__moduleInstanceConfig"]["__uuid"];

        
        SK_Window* wnd = wndMngr->newWindow();
      

        wnd->tag = wndID;
        wnd->ipc.sender_id = wndID;

        nlohmann::json constructorOpts = payload["constructorOpts"];
            
        if (SK_Global::GetInstance().onPreConfigWnd) SK_Global::GetInstance().onPreConfigWnd(wnd, constructorOpts);
           
        wnd->configWithInfo(constructorOpts);
            
        if (SK_Global::GetInstance().onPostConfigWnd) SK_Global::GetInstance().onPostConfigWnd(wnd);
            
        //newWnd->webview.navigate(SK_Base_URL + "/sk:view/" + wndID);

        if (wnd->config.data.contains("mainWindow") && wnd->config.data["mainWindow"] == true) {

            #if defined(SK_OS_windows)
                wnd->wndHandle = SK_Global::GetInstance().mainWindow->wndHandle;
            #elif defined(SK_OS_apple)
                #ifdef __OBJC__
                    wnd->wndHandle = SK_Global::GetInstance().mainWindow->wndHandle;
                    wnd->contentView = SK_Global::GetInstance().mainWindow->contentView;
                #endif
            #endif
                
            wnd->windowClassName = "SK_Window_" + wndID;
                
            SK_Global::GetInstance().setMainWindowSize(wnd->config["width"], wnd->config["height"]);

            wnd->createWebView();
        }
        else {
            wnd->create();
        }

        respondWith.JSON_OK();
    }

    void configure(SK_Window* wnd, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        
        SK_String attribute = payload["attribute"];

        if (payload.contains("read") && payload["read"] == true) {
           auto value = wnd->config.data[attribute];
           respondWith.JSON(value);
           return;
        }

        if (payload["value"].is_object()) {
           handleDetailedAttributeAssignment(wnd, attribute, payload["value"]);
        }
        else {
           wnd->config[attribute] = payload["value"];
        }

        respondWith.JSON_OK();
    }

    void handleDetailedAttributeAssignment(SK_Window* wnd, const SK_String& attribute, const nlohmann::json& value) {
        
        if (attribute == "setAlwaysOnTop"){
           /*wnd->setAlwaysOnTop(value["flag"], (value.contains("level") ? value["level"] : -1), (value.contains("relativeLevel") ? value["relativeLevel"] : -1));
            */
        }
    }
    
    
    
    void loadURL(SK_Window* wnd, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        wnd->webview.navigate(SK_Base_URL + SK_String(payload["url"]));
        respondWith.JSON_OK();
    }
};

END_SK_NAMESPACE
