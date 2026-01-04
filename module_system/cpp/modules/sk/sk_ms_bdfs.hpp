#pragma once

#include "../../../../core/sk_common.hpp"
#include "../../../../core/sk_web/sk_web_utils.hpp"

BEGIN_SK_NAMESPACE

class SK_Module_bdfs {
public:
    SK_Global* skg;

    SK_Module_bdfs(SK_Global* _skg) {
        skg = _skg;
        
        skg->bdfsEntryExistsByPath = [&](const SK_String& path){
            #if defined(SK_BUNDLE_MODE_DEEP) || defined(SK_BUNDLE_MODE_SHALLOW)
                SK_SoftBackend_Bundle_Entry_Info* entry = skg->bundle_library->findByPath(path);
                if (entry) return true;
            #endif
            
            return false;
        };
    }
    
    ~SK_Module_bdfs(){
        skg = nullptr;
    }


    #if defined(SK_BUNDLE_MODE_DEEP) || defined(SK_BUNDLE_MODE_SHALLOW)
        void handleOperation(const SK_String& operation, nlohmann::json& payload, SK_Communication_Response& respondWith) {
            SK_String _path = payload["path"];
            if (_path.length() == 0) payload["path"] = "/";
            
            
            SK_String path = SK_String(std::filesystem::path(SK_String(payload["path"])).lexically_normal().string()).replaceAll("\\", "/");
            if (path.length() > 1 && path.substring(path.length() - 1, 1) == "/") path = path.substring(0, path.length() - 1);
            
            SK_String data = (payload.contains("data") ? payload["data"] : "");

                 if (operation == "access") access(path, respondWith);
            else if (operation == "stat") stat(path, respondWith);
            else if (operation == "writeFile") writeFile(path, data, respondWith);
            else if (operation == "readFile") readFile(path, respondWith);
            else if (operation == "readdir") readdir(path, respondWith);
            else if (operation == "readJSON") readJSON(path, respondWith);
            else if (operation == "writeJSON") writeJSON(path, data, respondWith);
        }


        void access(const SK_String& path, SK_Communication_Response& respondWith) {

            SK_SoftBackend_Bundle_Entry_Info* entry = skg->bundle_library->findByPath(path);

            if (!entry) {
                respondWith.error(404, "ENOENT");
                return;
            }

            respondWith.JSON({});
        }

        void stat(const SK_String& path, SK_Communication_Response& respondWith) {
            SK_SoftBackend_Bundle_Entry_Info* entry = skg->bundle_library->findByPath(path);

            if (!entry) {
                respondWith.error(404, "ENOENT");
                return;
            }

            respondWith.JSON({
                {"type", (entry->isFolder ? "dir" : "file")},
                {"dev", -1},
                {"mode", -1},
                {"nlink", 1},
                {"uid", 0},
                {"gid", 0},
                {"rdev", 0},
                {"blksize", -1},
                {"ino", 0},
                {"size", entry->size},
                {"blocks", -1},
                {"atimeMs", ""},
                {"mtimeMs", ""},
                {"ctimeMs", ""},
                {"birthtimeMs", ""},
                {"atime"    , ""},
                {"mtime"    , ""},
                {"ctime"    , ""},
                {"birthtime", ""}
            });
        }

        void writeFile(const SK_String& path, const SK_String& data, SK_Communication_Response& respondWith) {
            respondWith.error(501, "ENOSYS");
        }

        void readFile(const SK_String& path, SK_Communication_Response& respondWith) {
            SK_SoftBackend_Bundle_Entry_Info* entry = skg->bundle_library->findByPath(path);

            if (entry->isFolder) {
                respondWith.error(404, "ENOENT");
                return;
            }

            SK::SK_String data = entry->dataAs_SKString();
            respondWith.JSON({ {"data", data.toBase64()} });
        }

        void readdir(const SK_String& path, SK_Communication_Response& respondWith) {
            SK_SoftBackend_Bundle_Entry_Info* entry = skg->bundle_library->findByPath(path);

            if (!entry || !entry->isFolder) {
                respondWith.error(404, "ENOENT");
                return;
            }
        
            respondWith.JSON(entry->readDir());
        }

        void readJSON(const SK_String& path, SK_Communication_Response& respondWith) {
            respondWith.JSON_OK();
        }

        void writeJSON(const SK_String& path, const SK_String& data, SK_Communication_Response& respondWith) {
            respondWith.error(501, "ENOSYS");
        }
    #elif defined(SK_BUNDLE_MODE_NONE)
        void handleOperation(const SK_String& operation, nlohmann::json& payload, SK_Communication_Response& respondWith) {
            respondWith.error(404, "ENOENT");
        }
    #endif
};
END_SK_NAMESPACE
