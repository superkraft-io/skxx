#pragma once

#include "../../../../../core/sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Module_vfs_file {
public:
    SK_String path;
    SK_String data;
    int ctime;
    int mtime;
    int atime;

    size_t getSize() const {
        return data.length();
    }


    #if defined(SK_FRAMEWORK_JUCE)
        WebBrowserComponent::Resource toResource() const {
            WebBrowserComponent::Resource resource;

            resource.data.resize(data.length());
            std::memcpy(resource.data.data(), data.data().c_str(), data.length());

            resource.mimeType = SK_Web_MIME_utils::fromFileExt(path);

            return resource;
        };
    #endif
};

END_SK_NAMESPACE
