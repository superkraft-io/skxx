#pragma once
#include <JuceHeader.h>

#include "handleParamMouseEvent/sk_ms_cNA_handleParamMouseEvent.h"

#include "../../../../../../../../../sk_nativeActions/sk_nativeActions.h"

class SK_VB_CoreNativeActions {
public:
    SK_MS_cNA_handleParamComponentMouseEvent handleParamComponentMouseEvent;
    SK_Project_NativeActions projectNativeActions;

    void handle_IPC_Msg(String msgID, DynamicObject* obj, String& responseData);
};
