#pragma once

#include "../../../../../core/sk_common.hxx"

BEGIN_SK_NAMESPACE

class SK_Module_vfs {
public:
    std::vector<SK_Module_vfs_file*> entries;

    SK_Module_vfs::~SK_Module_vfs() {
        for (int i = 0; i < entries.size(); i++) delete entries[i];
    }

    void handleOperation(const SK_String& operation, const SK_JSON_YY& payload, SK_Communication_Response& respondWith) {
        SK_String path = payload["path"];
        SK_String data = payload["data"];


        if (operation == "access") access(path, respondWith);
        else if (operation == "stat") stat(path, respondWith);
        else if (operation == "writeFile") writeFile(path, data, respondWith);
        else if (operation == "readFile") readFile(path, respondWith);
        else if (operation == "readdir") readdir(path, respondWith);
        else if (operation == "readJSON") readJSON(path, respondWith);
        else if (operation == "writeJSON") writeJSON(path, data, respondWith);
    };

    SK_Module_vfs_file* findByPath(const SK_String& path) {
        SK_String _path = path;
        
        if (path.substring(0, 1) == "/") _path = path.substring(1, path.length());

        for (int i = 0; i < entries.size(); i++) {
            SK_Module_vfs_file* file = entries[i];
            if (file->path == _path) return entries[i];
        }

        return nullptr;
    };

    void access(const SK_String& path, SK_Communication_Response& respondWith) {
        SK_Module_vfs_file* file = findByPath(path);

        respondWith.JSON(SK_JSON_YY{
            {"data", (file != nullptr ? true : false)}
        });
    }

    void stat(const SK_String& path, SK_Communication_Response& respondWith) {
        SK_Module_vfs_file* file = findByPath(path);

        if (file == nullptr) {
            respondWith.error(404, "ENOENT");
            return;
        }


        auto statInfo = SK_JSON_YY {
            {"type", "file"},
            {"dev", -1},
            {"mode", -1},
            {"nlink", 1},
            {"uid", 0},
            {"gid", 0},
            {"rdev", 0},
            {"blksize", -1},
            {"ino", 0},
            {"size", file->getSize()},
            {"blocks", -1},
            {"atimeMs", file->atime},
            {"mtimeMs", file->mtime},
            {"ctimeMs", file->ctime},
            {"birthtimeMs", file->ctime},
            {"atime"    , ""},
            {"mtime"    , ""},
            {"ctime"    , ""},
            {"birthtime", ""}
        };

        respondWith.JSON(statInfo);
    };

    void writeFile(const SK_String& path, const SK_String& data, SK_Communication_Response& respondWith) {
        entries.push_back(new SK_Module_vfs_file());

        SK_Module_vfs_file* file = entries.at(entries.size() - 1);
        file->path = path;
        file->data = data;

        respondWith.JSON_OK();
    };

    void readFile(const SK_String& path, SK_Communication_Response& respondWith) {
        SK_Module_vfs_file* file = findByPath(path);

        if (file == nullptr) {
            respondWith.error(404, "ENOENT");
            return;
        }

        respondWith.JSON(SK_JSON_YY{
            {"data", file->data}
        });
    };

    void readdir(const SK_String& path, SK_Communication_Response& respondWith) {
        respondWith.JSON_OK();
    };

    void readJSON(const SK_String& path, SK_Communication_Response& respondWith) {
        respondWith.JSON_OK();
    };

    void writeJSON(const SK_String& path, const SK_String& data, SK_Communication_Response& respondWith) {
        respondWith.JSON_OK();
    };
};
END_SK_NAMESPACE