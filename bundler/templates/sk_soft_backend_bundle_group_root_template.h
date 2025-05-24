#pragma once

BEGIN_SK_NAMESPACE

using SK_SoftBackend_Bundle_Data_Group_Root_GetPointers_CB = std::function<void(void* offsets, void* sizes, void* data, size_t* data_size)>;

class SK_SoftBackend_Bundle_Data_Group_Root {
public:
    SK_SoftBackend_Bundle_Data_Group_Root_GetPointers_CB getPointersCB;

    virtual void getPointers(void* offsets, void* sizes, void* data, size_t* data_size) {
        if (getPointersCB) {
            getPointersCB(offsets, sizes, data, data_size);
        }
    };
};

END_SK_NAMESPACE
