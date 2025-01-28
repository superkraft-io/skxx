#pragma once

#include "../../../../../core/sk_common.hxx"

BEGIN_SK_NAMESPACE

class SK_Module_viewMngr {
public:
    SK_Window_Mngr* wndMngr;

    

    void handleOperation(const SK_String& operation, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
             if (operation == "create") createView(payload, respondWith);
        //else if (operation == "setBGClr") setBGClr(payload, respondWith);
    };

    void createView(const nlohmann::json& payload, SK_Communication_Response& respondWith) {


        respondWith.JSON_OK();
        return;

        SK_String viewID = payload["id"];

        SK_Window* existingView = wndMngr->findWindowByTag(viewID);
        if (existingView != nullptr) return;

        SK_String _payload = payload.dump(4);

        SK_Window* wnd = wndMngr->newWindow([&](SK_Window* wnd) {
            wnd->tag = viewID;
            wnd->ipc.sender_id = viewID;

            wnd->configWithInfo(payload);
            
            wnd->webview.navigate("https://sk.project/sk_vfs/sk_project/views/" + viewID + "/frontend/view.html");

            if (payload.contains("mainWindow") && payload["mainWindow"]) {
                wnd->config["mainWindow"] = true;

                wnd->hwnd = SK_Common::mainWindowHWND;
                wnd->windowClassName = "SK_Window_1";

                SK_Common::setMainWindowSize(payload["width"], payload["height"]);

                wnd->createWebView();
            }
            else {
                wnd->create();
            }
        });


        //int minWidth = payload["minWidth"] || -1;
        //int minHeight = payload["minHeight"] || -1;

        //int maxWidth = payload["maxWidth"] || -1;
        //int maxHeight = payload["maxHeight"] || -1;

        /*
        SK_String iconPath = payload["icon"] || "N/A";

        bool maximizable = payload["maximizable"] || true;
        bool minimizable = payload["minimizable"] || true;

        */





        respondWith.JSON_OK();
    }
};

END_SK_NAMESPACE