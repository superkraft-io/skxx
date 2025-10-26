#pragma once

#include "../../../../../core/sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Module_vfs_file {
public:
    SK_Global* skg;

    SK_String path;
    SK_String data;
    int ctime;
    int mtime;
    int atime;

    size_t getSize() const {
        return data.length();
    }
};

END_SK_NAMESPACE
