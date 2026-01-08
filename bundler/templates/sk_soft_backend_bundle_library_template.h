#pragma once

#include "sk_soft_backend_bundle_include.h"
#include "sk_soft_backend_bundle_groups.h"
#include "sk_soft_backend_bundle_entry.h"
#include "sk_soft_backend_bundle_entries_files.h"
#include "sk_soft_backend_bundle_entries_folders.h"

BEGIN_SK_NAMESPACE


class SK_SoftBackend_Bundle_Library {
public:
    SK_SoftBackend_Bundle_Library_Groups groups;
    SK_SoftBackend_Bundle_Library_Files fileEntries;
    SK_SoftBackend_Bundle_Library_Folders folderEntries;
    
    SK_SoftBackend_Bundle_Library() : fileEntries(groups), folderEntries(groups) {}
    
    SK_SoftBackend_Bundle_Entry_Info* findByPath(const SK_String& path) {
        auto fileEntry_Pair = fileEntries.find(path);
        auto folderEntry_Pair = folderEntries.find(path);

        if (fileEntry_Pair == fileEntries.end() && folderEntry_Pair == folderEntries.end()) return nullptr;

        SK_SoftBackend_Bundle_Entry_Info* entry = nullptr;
        
        if (fileEntry_Pair != fileEntries.end()){
            entry = fileEntry_Pair->second;

            
            #if defined(SK_BUNDLE_MODE_SHALLOW)
                loadGroup(entry->groupIdx);
            #endif
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



    std::string pathBackwardsUntilNeighbour(const std::string& neighbourName, const SK_String& startPath = "") {
        std::filesystem::path currentPath(startPath);

        bool stop = false;
        while (!stop) {
            currentPath = currentPath.parent_path();

            if (currentPath == currentPath.root_path()) {
                return "";
            }

            try {
                for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
                    SK_String _path = SK_String(entry.path().filename().string());
                    
                    if (entry.is_directory() && _path.toLowerCase() == SK_String(neighbourName).toLowerCase()) {
                        stop = true;
                        break;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error accessing directory: " << e.what() << ". If you're on MacOS using Xcode, it may be due to sandboxin in your Xcode project settings. Remove sandboxing.\n";
            }
        }

        return currentPath.string();
    }


    #if defined(SK_BUNDLE_MODE_SHALLOW)
        bool loadGroup(int groupID){
            auto* group = static_cast<SK_SoftBackend_Bundle_Data_Group_Root*>(groups[groupID]);
            if (!group) {
                return false;
            }

            group->openedTime = std::time(nullptr);

            if (group->file){
                return true;
            }

            //Load shallow data
            SK_String bundle_name = "skxx_bundle";

            #if defined(SKXX_BUNDLE_NAME)
                bundle_name = SKXX_BUNDLE_NAME;
            #endif
            
            SK_String path = pathBackwardsUntilNeighbour(bundle_name, SK_Path_Utils::getCurrentProcessPath());

            if (path != "") {
                path = path + "/" + bundle_name + "/" + SK_String(groupID) + ".bin";
            }

            group->file = fopen(path.replaceAll("\\", "/").c_str(), "rb");
            if (group->file) {
                //...
            } else {
                path = SK_BUNDLER_SHALLOW_DATA_PATH + "/" + SK_String(groupID) + ".bin";
                group->file = fopen(path.replaceAll("\\", "/").c_str(), "rb");
            }
            
            if (group->file) {
                fseek(group->file, 0, SEEK_END);
                long dataSize = ftell(group->file);
                std::vector<unsigned char> buffer(dataSize);
                fseek(group->file, 0, SEEK_SET);
                fread(buffer.data(), 1, dataSize, group->file);
                group->data = std::move(buffer);

                return true;
            }

            throw std::runtime_error("[SK++] Failed to load file for shallow bundle group <!id!>\n");
        }

        void closeGroup(SK_SoftBackend_Bundle_Data_Group_Root* group){
            if (group->file) {
                fclose(group->file);
                group->file = nullptr;
            }
            group->openedTime = 0;
            group->data.clear();
            group->data.shrink_to_fit();
        }

        void flushIfExpired(SK_SoftBackend_Bundle_Data_Group_Root* group) {
            if (group->openedTime > 0) {
                time_t elapsed = std::time(nullptr) - group->openedTime;
                if (elapsed >= 5) {
                    closeGroup(group);
                }
            }
        }

        void flushCache() {
            // iterate through groups and call flush()
            for (void* p : groups) {
                auto* group = static_cast<SK_SoftBackend_Bundle_Data_Group_Root*>(p);
                if (group) {
                    flushIfExpired(group);
                }
            }
        }
    #endif
};

END_SK_NAMESPACE