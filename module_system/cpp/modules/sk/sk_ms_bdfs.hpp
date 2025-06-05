#pragma once

#include "../../../../core/sk_common.hpp"
#include "../../../../core/sk_web/sk_web_utils.hpp"

BEGIN_SK_NAMESPACE

class SK_Module_bdfs {
public:
    SK_Global* skg;

    SK_Module_bdfs(SK_Global* _skg) {
        skg = _skg;
    }
    
    ~SK_Module_bdfs(){
        skg = nullptr;
    }


    void handleOperation(const SK_String& operation, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        SK_String path = std::filesystem::path(payload["path"]).lexically_normal().string();
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
};
END_SK_NAMESPACE
