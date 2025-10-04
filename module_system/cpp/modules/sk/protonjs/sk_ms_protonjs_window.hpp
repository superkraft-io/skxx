#pragma once

#include "../../../../../core/sk_common.hpp"

#ifdef __OBJC__
    #import <AppKit/AppKit.h>
#endif

BEGIN_SK_NAMESPACE

class SK_Module_ProtonJS_Window {
public:
    SK_Global* skg;

    SK_Window_Mngr* wndMngr;
    
    SK_Module_ProtonJS_Window(SK_Global* _skg) {
        skg = _skg;
    }
    
    ~SK_Module_ProtonJS_Window() {
        wndMngr = nullptr;
        skg = nullptr;
    }
    
    void handleOperation(const SK_String& operation, nlohmann::json& payload, SK_Communication_Response& respondWith) {
        
        SK_String wndID = payload["__moduleInstanceConfig"]["__uuid"];
        SK_Window* wnd = wndMngr->findWindowByTag(wndID);
        
        
              if (operation == "construct") construct(wnd, payload, respondWith);
         else if (operation == "configure") configure(wnd, payload, respondWith);
         else if (operation == "loadURL") loadURL(wnd, payload, respondWith);
         else if (operation == "windowAction") windowAction(wnd, payload, respondWith);
    };

    void construct(SK_Window* _wnd, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        
        if (_wnd != nullptr) return;
        
   
        
        SK_String wndID = payload["__moduleInstanceConfig"]["__uuid"];

        
        SK_Window* wnd = wndMngr->newWindow();
      
        wnd->webview.parentWnd = wnd;

        wnd->tag = wndID;
        wnd->ipc->sender_id = wndID;

        nlohmann::json constructorOpts = payload["constructorOpts"];
            
        if (skg->onPreConfigWnd) skg->onPreConfigWnd(wnd, constructorOpts);
        
        //wnd->config.bypassCallback = true;
        wnd->configWithInfo(constructorOpts);
        //wnd->config.bypassCallback = false;
        
        if (skg->onPostConfigWnd) skg->onPostConfigWnd(wnd);
        
        
        if (wnd->config.data.contains("mainWindow") && wnd->config.data["mainWindow"] == true) {

            #if defined(SK_OS_windows)
                wnd->wndHandle = skg->mainWindow->wndHandle;
            #elif defined(SK_OS_apple)
                #ifdef __OBJC__
                    wnd->wndHandle = skg->mainWindow->wndHandle;
                    wnd->contentView = skg->mainWindow->contentView;
            
                    #if defined(SK_APP_TYPE_au)
                        //This code block causes issues in some DAW's, so we'll allow only fixed-size plugin windows for now
                        //wnd->contentView.translatesAutoresizingMaskIntoConstraints = false;
                        /*[NSLayoutConstraint activateConstraints:@[
                            [wnd->contentView.leadingAnchor constraintEqualToAnchor:wnd->contentView.superview.leadingAnchor],
                            [wnd->contentView.trailingAnchor constraintEqualToAnchor:wnd->contentView.superview.trailingAnchor],
                            [wnd->contentView.topAnchor constraintEqualToAnchor:wnd->contentView.superview.topAnchor],
                            [wnd->contentView.bottomAnchor constraintEqualToAnchor:wnd->contentView.superview.bottomAnchor]
                        ]];*/
                    #endif
                #endif
            #endif
            
            wnd->windowClassName = "SK_Window_" + wndID;
            
            skg->setMainWindowSize(wnd->config["width"], wnd->config["height"]);

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

    void windowAction(SK_Window* wnd, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        wnd->handleWindowAction(payload);
        respondWith.JSON_OK();
    }
};

END_SK_NAMESPACE
