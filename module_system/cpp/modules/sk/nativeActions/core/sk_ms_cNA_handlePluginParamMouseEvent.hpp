#pragma once

#include "../../../../../../core/sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_MS_cNA_handlePluginParamMouseEvent {
public:
    SK_Global* skg;

    void handleOperation(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        if (skg->handlePluginParamEvent) skg->handlePluginParamEvent(payload, respondWith);
        else respondWith.error();
    };
};

END_SK_NAMESPACE
