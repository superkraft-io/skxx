#pragma once


#include "../rezonant/skxx/core/sk_include.h"
#include "../rezonant/skxx/core/utils/sk_string.h"
#include "../rezonant/skxx/core/sk_communication/sk_communication_response.hpp"
#include "../rezonant/skxx/module_system/cpp/modules/sk/nativeActions/sk_nativeAction_root.hpp"

//<includes>

BEGIN_SK_NAMESPACE

class SK_App_NativeActions {
public:
    SK_Global* skg;


    //example entry: {"actionName", new SK_NativeAction_actionName()}
    std::unordered_map<std::string, void*> actions {
       //<entries>
    };



    SK_App_NativeActions(SK_Global* _skg) {
        skg = _skg;
        
        for (auto& pair : actions) {
            static_cast<SK_NativeAction_Root*>(pair.second)->skg = skg;
        }
    }

    ~SK_App_NativeActions() {
        for (auto& pair : actions) {
            delete pair.second;
        }
        actions.clear();
    }
    

    bool handleOperation(const SK::SK_String& operation, nlohmann::json& payload, SK::SK_Communication_Response& respondWith) {
        void* action = actions[operation];

        if (!action) return false;

        static_cast<SK::SK_NativeAction_Root*>(action)->run(payload, respondWith);

        return true;
    };
};

END_SK_NAMESPACE
