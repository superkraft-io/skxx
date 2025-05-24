#pragma once

#include "../skxx/core/sk_var.hpp"
#include "../skxx/core/utils/sk_string.h"
#include "../skxx/core/sk_web/sk_web_utils.hpp"

BEGIN_SK_NAMESPACE

class SK_SoftBackend_Bundle_Entry {
public:
    bool isFolder = false;

    SK_String path     = "";
    SK_String filename = "";
    SK_String mimeType = "";

    size_t size = -1;
    size_t offset = -1;

    SK_String folders = "";
    SK_String files = "";
    
    void* dataPtr;

    SK_SoftBackend_Bundle_Entry(
        const bool _isFolder,
        const SK_String& _path,
        const SK_String& _filename,
        size_t _size,
        const SK_String& _folders,
        const SK_String& _files,
        void* _dataPtr
    ): isFolder(_isFolder),
    path(_path),
    filename(_filename),
    mimeType(SK_Web_MIME_utils::GetInstance().fromFilename(_filename)),
    size(_size),
    offset(_size),
    folders(_folders),
    files(_files)
    {
        dataPtr = _dataPtr;
    }

    ~SK_SoftBackend_Bundle_Entry() {
        free(dataPtr);
        dataPtr = nullptr;
    }

    char* dataAsChar() {
        if (dataPtr == nullptr) return nullptr;
        return static_cast<char*>(dataPtr);
    }
};

END_SK_NAMESPACE
