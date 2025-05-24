#pragma once

#include "../sk_soft_backend_bundle_group_root.h"

BEGIN_SK_NAMESPACE

class SK_SoftBackend_Bundle_Data_Group_<!id!> : public SK_SoftBackend_Bundle_Data_Group_Root {
public:
    const size_t offsets[<!offsets_arr_size!>] = {<!offsets!>};
    const size_t sizes[<!sizes_arr_size!>] = {<!sizes!>};
    
    const size_t data_size = <!data_size!>;
    const unsigned char data[<!data_size!>] = {<!data!>};

    SK_SoftBackend_Bundle_Data_Group_<!id!>(){
        getPointersCB = [this](void* _offsets, void* _sizes, void* _data, size_t* _data_size) {
            _offsets = (size_t*)this->offsets;
            _sizes = (size_t*)this->sizes;
            _data = (unsigned char*)this->data;
            _data_size = &this->data_size;
        };
    };
};

END_SK_NAMESPACE
