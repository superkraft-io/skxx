#pragma once

#include "../../../../../core/sk_common.hpp"

BEGIN_SK_NAMESPACE

using SK_NativeAction_Root_CB = std::function<void(const nlohmann::json& payload, SK_Communication_Response& respondWith)>;

class SK_NativeAction_Root {
public:
    SK_NativeAction_Root_CB run;
};

END_SK_NAMESPACE
