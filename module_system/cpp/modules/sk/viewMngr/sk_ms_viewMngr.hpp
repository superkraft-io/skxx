#pragma once

#include "../../../../../core/sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Module_viewMngr {
public:
    SK_Window_Mngr* wndMngr;

    

    void handleOperation(const SK_String& operation, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
             if (operation == "create") createView(payload, respondWith);
        //else if (operation == "setBGClr") setBGClr(payload, respondWith);
    };

    void createView(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        SK_String viewID = payload["id"];

        SK_Window* existingView = wndMngr->findWindowByTag(viewID);
        if (existingView != nullptr) return;

        SK_Window* wnd = wndMngr->newWindow([&](SK_Window* wnd) {
            wnd->tag = viewID;
            wnd->ipc.sender_id = viewID;

            wnd->configWithInfo(payload);
           
            //wnd->webview.navigate("https://sk.project/sk_vfs/sk_project/views/" + viewID + "/frontend/view.html");

            if (wnd->config.data.contains("mainWindow") && wnd->config.data["mainWindow"] == true) {

                #if defined(SK_OS_windows)
                    wnd->hwnd = SK_Global::mainWindowHWND;
                #elif defined(SK_OS_macos)
                #endif
                
                wnd->windowClassName = "SK_Window_1";

                SK_Global::setMainWindowSize(wnd->config["width"], wnd->config["height"]);

                wnd->createWebView();
            }
            else {
                wnd->create();
            }
        });


        respondWith.JSON_OK();
    }
};

END_SK_NAMESPACE
