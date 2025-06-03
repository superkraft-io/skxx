#pragma once

#include "../../sk_soft_backend_bundle_group_root.h"

#if defined(SK_BUNDLER_MODE_SHALLOW)
    #include <filesystem>
#endif

BEGIN_SK_NAMESPACE

class SK_SoftBackend_Bundle_Data_Group_<!id!> : public SK_SoftBackend_Bundle_Data_Group_Root {
public:
    size_t offsets[<!offsets_arr_size!>] = {<!offsets!>};
    size_t sizes[<!sizes_arr_size!>] = {<!sizes!>};
    
    size_t data_size = <!data_size!>;
    unsigned char data[<!data_size!>] = {<!data!>};

    SK_SoftBackend_Bundle_Data_Group_0() {
        getPointersCB = [this](void** _offsets, void** _sizes, void** _data, size_t* _data_size) {
            *_offsets = (void*)this->offsets;  // Assign address of `offsets`
            *_sizes = (void*)this->sizes;     // Assign address of `sizes`
            *_data = (void*)this->data;       // Assign address of `data`
            *_data_size = this->data_size;    // Assign data size
        };

        #if defined(SK_BUNDLER_MODE_SHALLOW)
            loadShallowData();
        #endif
    };

    bool loadShallowData(){
        //Load shallow data
        auto file = std::filesystem::path(SK_BUNDLE_SHALLOW_DATA_PATH) / "group_<!id!>.bin";

        FILE* file = fopen(path.replaceAll("\\", "/").c_str(), "rb");
		if (file) {
			fseek(file, 0, SEEK_END);
			long dataSize = ftell(file);
			char* buffer = (char*)malloc(dataSize + 1);
			fseek(file, 0, SEEK_SET);
			fread(buffer, 1, dataSize, file);

			data.resize(dataSize);
			std::memcpy(data.data(), buffer, dataSize);

			free(buffer);

			fclose(file);

            return true;
        }

        return false;
    }
};

END_SK_NAMESPACE
