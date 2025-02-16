#pragma once

#include "../../../../../core/sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Module_ProtonJS {
public:
    SK_Window_Mngr* wndMngr;

    void handleOperation(const SK_String& operation, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
             if (operation == "construct") construct(payload, respondWith);
        else if (operation == "configure") configure(payload, respondWith);
    };

    void construct(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        SK_String uuid = payload["__moduleInstanceConfig"]["__uuid"];

        SK_Window* existingView = wndMngr->findWindowByTag(uuid);
        if (existingView != nullptr) return;

        SK_Window* wnd = wndMngr->newWindow([&](SK_Window* wnd) {
            wnd->tag = uuid;
            wnd->ipc.sender_id = uuid;

            wnd->configWithInfo(payload["constructorOpts"]);

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

    void configure(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        SK_String uuid = payload["__moduleInstanceConfig"]["__uuid"];

        SK_Window* wnd = wndMngr->findWindowByTag(uuid);

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
};

END_SK_NAMESPACE
