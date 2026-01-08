#pragma once

#ifndef __INTELLISENSE__ // Disable for Intellisense

#include "../../bundle_group_root.h"

BEGIN_SK_NAMESPACE

class SK_SoftBackend_Bundle_Data_Group_<!id!> : public SK_SoftBackend_Bundle_Data_Group_Root {
public:
    unsigned int groupID = <!id!>;
    
    size_t offsets[<!offsets_arr_size!>] = {<!offsets!>};
    size_t sizes[<!sizes_arr_size!>] = {<!sizes!>};
    
    size_t data_size = <!data_size!>;

    #if defined(SK_BUNDLE_MODE_DEEP)
        unsigned char data[<!data_size!>] = {<!data!>};
    #endif



    SK_SoftBackend_Bundle_Data_Group_<!id!>() {
        
        getPointersCB = [this](void** _offsets, void** _sizes, void** _data, size_t* _data_size) {
            *_offsets = (void*)this->offsets;  // Assign address of `offsets`
            *_sizes = (void*)this->sizes;     // Assign address of `sizes`


            
            #if defined(SK_BUNDLE_MODE_SHALLOW)
                *_data_size = this->data.size();    // Assign data size
                *_data = (void*)this->data.data();       // Assign address of `data`
            #endif

            #if defined(SK_BUNDLE_MODE_DEEP)
                *_data_size =  this->data_size;    // Assign data size
                *_data = (void*)this->data;       // Assign address of `data`
            #endif
           
        };
    };

    ~SK_SoftBackend_Bundle_Data_Group_<!id!>() override {
        // No owned heap memory to free here; clear callback for safety.
        getPointersCB = nullptr;
    }
};

END_SK_NAMESPACE

#endif __INTELLISENSE__