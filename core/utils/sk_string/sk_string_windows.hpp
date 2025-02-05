#pragma once

#include "../../sk_common.hpp"
#include "sk_string_base.hpp"

BEGIN_SK_NAMESPACE

class SK_String : public SK_String_Base {
public:
    using SK_String_Base::SK_String_Base;
    
    SK_String(LPCWSTR wideStr) {
        if (wideStr == nullptr) {
            data = ""; // Handle null pointers safely
        }
        else {
            // Get the required buffer size for the conversion
            int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, nullptr, 0, nullptr, nullptr);

            if (bufferSize == 0) {
                data = ""; // Conversion failed
            }
            else {
                // Allocate a buffer for the converted string
                std::string result(bufferSize - 1, '\0'); // -1 to exclude the null terminator

                // Perform the conversion
                WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, result.data(), bufferSize, nullptr, nullptr);

                data = result; // Assign to the `data` member
            }
        }
    }
    
    operator const LPCWSTR () const {
        static thread_local std::wstring wideString; // Thread-local to avoid issues with temporary scope
        int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, data.c_str(), -1, nullptr, 0);
        wideString.resize(sizeNeeded - 1); // Exclude null terminator
        MultiByteToWideChar(CP_UTF8, 0, data.c_str(), -1, &wideString[0], sizeNeeded);
        return wideString.c_str();
    }
};

END_SK_NAMESPACE
