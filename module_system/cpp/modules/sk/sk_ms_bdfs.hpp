#pragma once

#include "../../../../core/sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Module_bdfs {
public:
    SK_Project_BinaryData* binaryData;

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
        /*responseData = "{\"access\": false}";

        auto pair = vbe->sk_bd.findEntryByPath(path);

        if (pair.first != "none") responseData = "{\"access\": true}";
        */
    }

    void stat(const SK_String& path, SK_Communication_Response& respondWith) {
        /*auto pair = vbe->sk_bd.findEntryByPath(path);

        if (pair.first == "none") {
            responseData = SK_IPC::Error("ENOENT");
            return;
        }

        SK_VB_BDFS_Entry* entry = (SK_VB_BDFS_Entry*)pair.second;

        auto statInfo = SSC::JSON::Object(SSC::JSON::Object::Entries{
            {"type", pair.first.toStdString()},
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
            {"atimeMs", entry->atime},
            {"mtimeMs", entry->mtime},
            {"ctimeMs", entry->ctime},
            {"birthtimeMs", entry->ctime},
            {"atime"    , ""},
            {"mtime"    , ""},
            {"ctime"    , ""},
            {"birthtime", ""}
            });


        auto tempstr = statInfo.str();

        responseData = String(tempstr);
        */
    }

    void writeFile(const SK_String& path, const SK_String& data, SK_Communication_Response& respondWith) {
        respondWith.JSON_OK();
    }

    void readFile(const SK_String& path, SK_Communication_Response& respondWith) {
        /*auto pair = vbe->sk_bd.findEntryByPath(path);

        if (pair.first == "dir") {
            responseData = SK_IPC::Error("ENOENT");
            return;
        }

        if (pair.first == "none") {
            responseData = "\"\"";
            return;
        }
        SK_VB_BDFS_File* entry = (SK_VB_BDFS_File*)pair.second;

        String fileData = entry->toBase64();

        responseData = "\"" + fileData + "\"";
        */
    }

    void readdir(const SK_String& path, SK_Communication_Response& respondWith) {
        //respondWith.JSON(binaryData->readDir(path));
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
