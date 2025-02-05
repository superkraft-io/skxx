#pragma once

#include "../../../../../../core/sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_MS_cNA_handleParamComponentMouseEvent {
public:
    void handleOperation(const nlohmann::json& payload, SK_Communication_Response& respondWith) {

        SK_String paramID = payload["paramID"];
        void* param;
        //RangedAudioParameter* param = NULL;//vbe->editor->m_processor.state.getParameter(juceParamID);

        if (param == NULL) {
            respondWith.error(404, "invalid_juce_param_id");
            return;
        }

        SK_String event = payload["event"];

        if (event == "read") {
            respondWith.JSON(nlohmann::json{
                {"value", 0}//SK_String(param->getValue()) }
            });
            //DBG(responseData);
            return;
        }


        if (event == "contextmenu") {
            if (SK_Common::runningAs == "app") {
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
            //const auto normalisedValue = param->convertTo0to1(value);
            //param->setValueNotifyingHost(normalisedValue);
        }


        respondWith.JSON_OK();
    };
};

END_SK_NAMESPACE
