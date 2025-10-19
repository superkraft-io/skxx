#pragma once

#ifndef __INTELLISENSE__ // Disable for Intellisense

#if defined(SK_BUNDLE_MODE_DEEP)
    <!deep_group_includes!>
#elif defined(SK_BUNDLE_MODE_SHALLOW)
    <!shallow_group_includes!>
#endif

BEGIN_SK_NAMESPACE


class SK_SoftBackend_Bundle_Entry_Info {
public:
    size_t idx = -1;
    size_t offset = -1;
    size_t size = -1;
    int groupIdx = -1;

    bool isFolder = false;
    SK_String filename;

    SK_String folderEntries;
    SK_String fileEntries;

    SK_SoftBackend_Bundle_Data_Group_Root* group;
    
    SK_SoftBackend_Bundle_Entry_Info(
        const size_t _offset,
        const size_t _size,
        const int _groupIdx,
        const size_t _idx,
        void* _group,
        bool _isFolder = false,
        const SK_String& _filename = "",
        const SK_String& _folderEntries = "",
        const SK_String& _fileEntries = ""
    ) : offset(_offset),    // Initialization list
        size(_size),
        groupIdx(_groupIdx),
        idx(_idx),
        group(static_cast<SK_SoftBackend_Bundle_Data_Group_Root*>(_group)),
        isFolder(_isFolder),
        filename(_filename),
        folderEntries(_folderEntries),
        fileEntries(_fileEntries)
    {
        // Constructor body (empty in this case)
    }
    
    SK_String dataAs_SKString() {
        void* _offsets;    // Will hold `offsets` array address
        void* _sizes;      // Will hold `sizes` array address
        void* _data;       // Will hold `data` array address
        size_t _data_size; // Will hold `data_size`

        // Pass addresses of the pointers (&_offsets, &_sizes, &_data)
        group->getPointers(&_offsets, &_sizes, &_data, &_data_size);

        size_t end = offset + size;
        if (end > _data_size) {
            throw std::runtime_error("Corrupted data: size exceeds buffer");
        }

        // Create string directly from the source range (zero-copy if possible)
        const char* data_start = static_cast<const char*>(_data) + offset;
        return SK_String(std::string(data_start, size));  // Efficient construction
    }
    
    nlohmann::json readDir(){
        nlohmann::json list = nlohmann::json::array();
        
        bool doSort = false;

        if (folderEntries.length() > 0){
            doSort = true;

            //add folders
            std::vector<std::string> folders = folderEntries.split(",");
            unsigned int folderEntries_size = folders.size();
            if (folderEntries_size > 0) {
                for (unsigned int i = 0; i < folderEntries_size; i++) {
                    std::string entryName = folders[i];
                    list.push_back(nlohmann::json{
                        {"type", "dir"},
                        {"name", entryName}
                    });
                }
            }
        }
        
        if (fileEntries.length() > 0){
            doSort = true;
            
            //add folders
            std::vector<std::string> files = fileEntries.split(",");
            unsigned int fileEntries_size = files.size();
            if (fileEntries_size > 0) {
                for (unsigned int i = 0; i < fileEntries_size; i++) {
                    std::string entryName = files[i];
                    list.push_back(nlohmann::json{
                        {"type", "dir"},
                        {"name", entryName}
                    });
                }
            }
        }
            
            
        if (doSort){
            std::sort(list.begin(), list.end(), [](const nlohmann::json& a, const nlohmann::json& b) {
                return a["name"] < b["name"];
            });
        }

        return list;
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

        if (fileEntry_Pair == fileEntries.end() && folderEntry_Pair == folderEntries.end()) return nullptr;

        SK_SoftBackend_Bundle_Entry_Info* entry = nullptr;
        
        if (fileEntry_Pair != fileEntries.end()){
            entry = fileEntry_Pair->second;
        }
        
        if (folderEntry_Pair != folderEntries.end()){
            entry = folderEntry_Pair->second;
        }
        
        return entry;
    };

    ~SK_SoftBackend_Bundle_Library() {
        // 1) Delete all data groups (polymorphic)
        for (void* p : groups) {
            auto* g = static_cast<SK_SoftBackend_Bundle_Data_Group_Root*>(p);
            delete g;
        }
        groups.clear();

        // Helper: dedup *within a single map* and delete each unique pointer once.
        auto dedup_and_delete_map = [](auto& m) {
            for (auto it1 = m.begin(); it1 != m.end(); ++it1) {
                auto* ptr = it1->second;
                if (!ptr) continue;
                // Null any later duplicates of the same pointer in this map
                for (auto it2 = std::next(it1); it2 != m.end(); ++it2) {
                    if (it2->second == ptr) it2->second = nullptr;
                }
                delete ptr;           // delete once
                it1->second = nullptr;
            }
            m.clear();
        };

        // 2) Avoid cross-map double free:
        //    If a pointer exists in fileEntries, let fileEntries own its deletion.
        for (auto& fk : folderEntries) {
            if (!fk.second) continue;
            for (auto& ek : fileEntries) {
                if (ek.second == fk.second) { fk.second = nullptr; break; }
            }
        }

        // 3) Delete unique pointers remaining in folderEntries
        dedup_and_delete_map(folderEntries);

        // 4) Delete unique pointers in fileEntries
        dedup_and_delete_map(fileEntries);
    }
};

END_SK_NAMESPACE

#endif // __INTELLISENSE__