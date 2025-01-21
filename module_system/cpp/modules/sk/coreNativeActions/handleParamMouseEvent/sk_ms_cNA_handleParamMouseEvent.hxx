#pragma once

#include "../../../../../../core/sk_var.hxx"
#include "../../../../../../core/sk_common.hxx"
#include "../../../../../../core/superkraft.hxx"
#include "../../../../../../core/utils/sk_path_utils.hxx"
#include "../../../../../../core/sk_webview_resourceHandler/os/sk_webview_resourceHandler_response_windows.hxx"
#include "../../../../../../core/utils/sk_string.hxx"
#include "../../../../../../core/utils/sk_file.hxx"
#include "../../../../../../core/utils/sk_machine.hxx"
#include "../../../../../../core/utils/sk_array.hxx"
#include "json.hpp"

class SK_VB_cNA_handleParamComponentMouseEvent {
public:
    void handleOperation(const SK_String& operation, const nlohmann::json& payload, SK_WebViewResource_Response& respondWith) {
        var info = obj->getProperty("data");

        SK_String juceParamID = payload["paramID"];
        RangedAudioParameter* param = vbe->editor->m_processor.state.getParameter(juceParamID);

        if (param == NULL) {
            respondWith.error(404, "invalid_juce_param_id");
            return;
        }

        String event = payload["event"];

        if (event == "read") {
            respondWith.JSON(nlohmann::json{
                {"value", 0}//SK_String(param->getValue()) }
            });
            //DBG(responseData);
            return;
        }


        if (event == "contextmenu") {
            if (Superkraft::runningAs == "standalone") {
                respondWith.error(404, "standalone_runtime");
                return;
            }

            int left = payload["left"];
            int top = payload["top"];

            #if defined(SK_FRAMEWORK_JUCE)
                auto ctx = vbe->editor->getHostContext();
                std::unique_ptr<juce::HostProvidedContextMenu> menu = ctx->getContextMenuForParameter(param);
                menu.get()->showNativeMenu(Points<int>{left, top});
            #elif defined(SK_FRAMEWORK_iPlug2)
                //WHY IS THIS NOT ACTIVATED???
                //wip
                int x = 0;
            #endif
        }


        //if (event == "mousedown") param->beginChangeGesture();
        //if (event == "mouseup") param->endChangeGesture();

        if (event == "write") {
            float value = payload["value"] || 0;
            const auto normalisedValue = param->convertTo0to1(value);
            param->setValueNotifyingHost(normalisedValue);
        }


        respondWith.JSON_OK();
    };
};
