#pragma once

#include "../../../../../../core/sk_common.hxx"

BEGIN_SK_NAMESPACE

class SK_MS_ViewMngr_View {
public:
    SK_String id;

    bool pageAboutToLoad(const SK_String& newUrl) {

    };

    void handle_IPC_message() {
        /*
        DynamicObject* obj = object.getDynamicObject();

        String target = obj->getProperty("target");
        String msgID = obj->getProperty("msgID");
        String cmd = obj->getProperty("cmd");

        String viewID = obj->getProperty("viewID");

        String packetStr = juce::JSON::toString(obj->getProperty("data")).replace("\r", "").replace("\n", "");


        SSC::JSON::Object reqInfoJson = SSC::JSON::Object::Entries{
            {"eventID", "sk.ipc"},
            {"viewID", viewID.toStdString()},
            {"data", packetStr.toStdString()}
        };

        String reqInfoStr = String(reqInfoJson.str());

        vbe->handle_ipc_msg_from_view(reqInfoStr);
        */
    }
};

END_SK_NAMESPACE