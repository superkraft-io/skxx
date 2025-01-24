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

    void resizeViews(int width, int height) {
        int offset = 0;
        
        #if defined(SK_DEBUG_MODE)
            offset = 16;
        #endif  

        /*
        for (int i = 0; i < views.size(); i++) {
            auto* view = views[i];
            view->setTopLeftPosition(offset, 0);
            view->setSize(width - offset, height);
        }
        */
    }

    void createView(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        SK_String viewID = payload["id"];

        SK_Window* existingView = wndMngr->findWindowByTag(viewID);
        if (existingView != nullptr) return;

        SK_String _payload = payload.dump(4);

        SK_Window* wnd = wndMngr->newWindow([&](SK_Window* wnd) {
            wnd->tag = viewID;
            wnd->ipc.sender_id = viewID;

            wnd->visible = true;

            wnd->webview.navigate("https://sk.project/sk_vfs/sk_project/views/" + viewID + "/frontend/view.html");

            if (payload.contains("mainWindow") && payload["mainWindow"]) {
                wnd->info["mainWindow"] = true;

                wnd->hwnd = SK_Common::mainWindowHWND;
                wnd->windowClassName = "SK_Window_1";

                SK_Common::setMainWindowSize(payload["width"], payload["height"]);


                wnd->createWebView();
            }
            else {
                wnd->info["mainWindow"] = false;
                wnd->create();
            }
        });


        //if (payload["mainWindow"]){
        //int width = payload["width"];
        //int height = payload["height"] || 480;

        //int minWidth = payload["minWidth"] || -1;
        //int minHeight = payload["minHeight"] || -1;

        //int maxWidth = payload["maxWidth"] || -1;
        //int maxHeight = payload["maxHeight"] || -1;

        /*
        SK_String iconPath = payload["icon"] || "N/A";

        bool maximizable = payload["maximizable"] || true;
        bool minimizable = payload["minimizable"] || true;


        bool sameWindow = payload["sameWindow"] || false;
        */



        //if (sameWindow) {



            //SK_String fullPath = juce::WebBrowserComponent::getResourceProviderRoot();// +indexPath;
            //view->goToURL(fullPath);

            /*juce::Timer::callAfterDelay(10000, [this]() {
                //String fullPath = juce::WebBrowserComponent::getResourceProviderRoot();// +indexPath;
                //view->goToURL(fullPath);
            });*/

            //vbe->getParentComponent()->addAndMakeVisible(view);


            //vbe->getParentComponent()->setSize(width, height);


        //}


        respondWith.JSON_OK();
    }
};

END_SK_NAMESPACE