#pragma once

#include "unordered_map"
#include "../../core/sk_common.hxx"
#include "../../core/utils/sk_string.hxx"


using SK_Module_Base_Callback = std::function<void(const nlohmann::json& data)>;

class SK_Module_Base_Class {
public:
    std::unordered_map<std::string, SK_Module_Base_Callback> operations;

    void addOperation(const SK_String& name, const SK_Module_Base_Callback& cb) {
        operations[name] = cb;
    }
};
