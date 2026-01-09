#pragma once
#include "../../bundle_group_root.h"

BEGIN_SK_NAMESPACE

#if defined(SK_BUNDLE_MODE_DEEP)
extern const unsigned char* const group_<!id!>_data;  // pointer to the bytes
extern const size_t group_<!id!>_data_size;           // total size
extern size_t* group_<!id!>_offsets;                  // pointer to offsets array
extern size_t* group_<!id!>_sizes;                    // pointer to sizes array
#endif

class SK_SoftBackend_Bundle_Data_Group_<!id!> : public SK_SoftBackend_Bundle_Data_Group_Root {
public:
    unsigned int groupID = <!id!>;

    SK_SoftBackend_Bundle_Data_Group_<!id!>() {
        getPointersCB = [this](void** _offsets, void** _sizes, void** _data, size_t* _data_size) {
            *_offsets = (void*)group_<!id!>_offsets;
            *_sizes   = (void*)group_<!id!>_sizes;

            #if defined(SK_BUNDLE_MODE_SHALLOW)
                *_data_size = this->data.size();
                *_data      = (void*)this->data.data();
            #endif

            #if defined(SK_BUNDLE_MODE_DEEP)
                *_data_size = group_<!id!>_data_size;
                *_data      = (void*)group_<!id!>_data;
            #endif
        };
    }

    ~SK_SoftBackend_Bundle_Data_Group_<!id!>() override {
        getPointersCB = nullptr;
    }
};

END_SK_NAMESPACE