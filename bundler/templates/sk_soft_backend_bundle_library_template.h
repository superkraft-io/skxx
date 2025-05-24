#pragma once

<!group_includes!>

BEGIN_SK_NAMESPACE


class SK_SoftBackend_Bundle_Entry_Info {
public:
    size_t idx = -1;
    size_t offset = -1;
    size_t size = -1;
    int groupIdx = -1;

    bool isFolder = false;
    SK_String filename;

    SK_String folders;

    SK_SoftBackend_Bundle_Data_Group_Root* group;
    
    SK_SoftBackend_Bundle_Entry_Info(
        const size_t _offset,
        const size_t _size,
        const int _groupIdx,
        const size_t _idx,
        void* _group,
        bool _isFolder = false,
        const SK_String& _filename = "",
        const SK_String& _folderEntries = ""
    ) : offset(_offset),    // Initialization list
        size(_size),
        groupIdx(_groupIdx),
        idx(_idx),
        group(static_cast<SK_SoftBackend_Bundle_Data_Group_Root*>(_group)),
        isFolder(_isFolder),
        filename(_filename),
        folders(_folderEntries)
    {
        // Constructor body (empty in this case)
    }
    
    char* dataAs_CharPtr(){
        void* _offsets;
        void* _sizes;
        void* _data;
        size_t _data_size;

        group->getPointers(_offsets, _sizes, _data, &_data_size);
        
        size_t end = offset + size;

        if (end > _data_size) {
            throw std::runtime_error("Corrupted data: size exceeds buffer");
        }

        return reinterpret_cast<char*>(static_cast<char*>(_data) + offset);
    }
};

class SK_SoftBackend_Bundle_Library {
public:
    std::vector<void*> groups = {
<!groups!>
    };

    std::map<std::string, SK_SoftBackend_Bundle_Entry_Info*> fileEntries {
<!file_entries!>
    };

    std::map<std::string, SK_SoftBackend_Bundle_Entry_Info*> folderEntries {
<!folder_entries!>
    };

    SK_SoftBackend_Bundle_Entry_Info* findByPath(const SK_String& path) {
        auto fileEntry_Pair = fileEntries.find(path);
        auto folderEntry_Pair = folderEntries.find(path);

        if (fileEntry_Pair == fileEntries.end() && fileEntry_Pair == fileEntries.end()) return nullptr;

        return (fileEntry_Pair ? fileEntry_Pair->second : folderEntry_Pair->second);
    };
};

END_SK_NAMESPACE
