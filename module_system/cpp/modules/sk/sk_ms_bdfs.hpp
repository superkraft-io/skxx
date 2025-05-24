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
        SK_String path = payload["path"];
        SK_String data = payload["data"];

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
        respondWith.JSON({"access", (entry ? true : false)});
        
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
        respondWith.JSON_OK();
    }

    void readFile(const SK_String& path, SK_Communication_Response& respondWith) {
        SK_SoftBackend_Bundle_Entry_Info* entry = skg->bundle_library->findByPath(path);

        if (entry->isFolder) {
            respondWith.error(404, "ENOENT");
            return;
        }

        respondWith.fileFromBuffer(entry->dataAs_CharPtr(), SK_Web_MIME_utils::GetInstance().fromFilename(entry->filename));
    }

    void readdir(const SK_String& path, SK_Communication_Response& respondWith) {
        respondWith.JSON(SK_SoftBackend_Bundle_Class::GetInstance().readDir(path));
        respondWith.JSON_OK();
    }

    void readJSON(const SK_String& path, SK_Communication_Response& respondWith) {
        respondWith.JSON_OK();
    }

    void writeJSON(const SK_String& path, const SK_String& data, SK_Communication_Response& respondWith) {
        respondWith.JSON_OK();
    }
};
END_SK_NAMESPACE
