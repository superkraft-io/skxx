#pragma once

BEGIN_SK_NAMESPACE

class SK_SoftBackend_Bundle_Library_Groups : public std::vector<void*> {
public:
    SK_SoftBackend_Bundle_Library_Groups() : std::vector<void*>{
        <!groups!>
    }{
        //...
    }
};

END_SK_NAMESPACE