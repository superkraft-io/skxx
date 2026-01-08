#pragma once

#include "sk_soft_backend_bundle_entry.h"

#ifndef __INTELLISENSE__

BEGIN_SK_NAMESPACE

class SK_SoftBackend_Bundle_Library_<!type!> : public std::map<std::string, SK_SoftBackend_Bundle_Entry_Info*> {
public:
    SK_SoftBackend_Bundle_Library_<!type!>(SK_SoftBackend_Bundle_Library_Groups& groups) : std::map<std::string, SK_SoftBackend_Bundle_Entry_Info*>{
        <!entries!>
    }{
        //...
    }
};

END_SK_NAMESPACE

#endif __INTELLISENSE__