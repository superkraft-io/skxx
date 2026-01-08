#pragma once

BEGIN_SK_NAMESPACE

using SK_SoftBackend_Bundle_Data_Group_Root_GetPointers_CB = std::function<void(void** offsets, void** sizes, void** data, size_t* data_size)>;
using SK_SoftBackend_Bundle_Data_Group_Root_FlushCache_CB = std::function<void()>;

class SK_SoftBackend_Bundle_Data_Group_Root {
public:
    SK_SoftBackend_Bundle_Data_Group_Root_GetPointers_CB getPointersCB;
    SK_SoftBackend_Bundle_Data_Group_Root_FlushCache_CB flushCacheCB;

    #if defined(SK_BUNDLE_MODE_SHALLOW)
       std::vector<unsigned char> data;

        FILE* file = nullptr;
        time_t openedTime = 0;
    #endif
    

    virtual void getPointers(void** offsets, void** sizes, void** data, size_t* data_size) {
        if (getPointersCB) {
            getPointersCB(offsets, sizes, data, data_size);
        }
    };

     virtual ~SK_SoftBackend_Bundle_Data_Group_Root() {
        getPointersCB = nullptr;
    }
};

END_SK_NAMESPACE