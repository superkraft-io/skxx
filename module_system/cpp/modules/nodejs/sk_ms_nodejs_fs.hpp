#pragma once

#include "../../../../core/sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Module_fs {
public:
    SK_Module_vfs* vfs;

    void handleOperation(const SK_String& operation, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        SK_String path = payload["path"];

        //If path starts with sk_vfs/, we route the operation to the VFS module
        if (path.substring(0, 7) == "sk_vfs/") {
            vfs->handleOperation(operation, payload, respondWith);
            return;
        }

        //!!! IMPORTANT !!!! If in RELEASE mode, we route the operation to the BDFS module
        #ifdef SK_MODE_RELEASE
            //if (vbe->mode != "debug") {
            //    vbe->sk_c_api->sk->bdfs->handle_IPC_Msg(msgID, obj, responseData);
            //    return;
           // }
        #endif

        SK_String fullPath = path;


        //If the path is not absolute, then make the SK_Project folder the root folder
        if (path.substring(0, 1) == "/" || !SK_File::isPathAbsolute(path)) {
            if (operation != "mkdir") {
                SK_String targetPrefix = path.substring(0, path.indexOf("/"));
                if (targetPrefix == "sk.modsys") {
                    fullPath = SK_Path_Utils::paths["module_system"] + path.replace("sk.modsys", "");
                }
                else {
                    fullPath = SK_Path_Utils::paths["project"] + path;
                }
            }
        }

        
             if (operation == "access"   ) access(fullPath, respondWith);
        else if (operation == "stat"     ) stat(fullPath, respondWith);
        else if (operation == "writeFile") writeFile(fullPath, payload["data"].get<std::string>(), respondWith);
        else if (operation == "readFile" ) readFile(fullPath, respondWith);
        else if (operation == "readdir"  ) readdir(fullPath, respondWith);
        else if (operation == "unlink"   ) unlink(fullPath, respondWith);
        else if (operation == "mkdir"    ) mkdir(fullPath, respondWith);
    };



    
    void access(const SK_String& path, SK_Communication_Response& respondWith) {
        respondWith.JSON(nlohmann::json{ {"access", SK_String(SK_File::exists(path) ? "true" : "false")} });
    };

    void writeFile(const SK_String& path, const SK_String& data, SK_Communication_Response& respondWith) {
        if (SK_File::exists(path)) SK_File::unlink(path);

        SK_File file;

        bool writeStatus = SK_File::append(path, data);

        if (writeStatus) {
            respondWith.JSON_OK();
        }
        else {
            respondWith.error(404, "ENOENT");
        }

    };

    void readFile(const SK_String& path, SK_Communication_Response& respondWith) {
        SK_File file;
        if (!file.loadFromDisk(path)) {
            respondWith.error(404, "ENOENT");
            return;
        }

        respondWith.JSON({ {"data", file.toBase64()} });
    };

    void readdir(const SK_String& path, SK_Communication_Response& respondWith) {
        nlohmann::json list = SK_File::list(path);

        if (list == false) {
            respondWith.error(404, "ENOENT");
            return;
        }

        nlohmann::json fileList = nlohmann::json::array();

        unsigned int size = list.size();
        if (size > 0) {
            for (unsigned int i = 0; i < size; i++) {
                nlohmann::json file = list[i];
                fileList.push_back(nlohmann::json{
                    {"type", file["type"]},
                    {"name", file["name"]}
                });
            }
        }

        respondWith.JSON(fileList);
    };

    void unlink(const SK_String& path, SK_Communication_Response& respondWith) {
        try {
            if (SK_File::unlink(path)) {
                respondWith.JSON_OK();
            }
            else {
                respondWith.error(404, "EBUSY");
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            respondWith.error(404, e.what());
        }
    };

    void mkdir(const SK_String& path, SK_Communication_Response& respondWith) {
        try {
            // Use create_directories to create the directory and all its parent directories
            if (std::filesystem::create_directories(path)) {
                respondWith.JSON_OK();
            }
            else {
                respondWith.error(404, "EBUSY");
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            respondWith.error(404, e.what());
        }
    };
    
    void stat(const SK_String& path, SK_Communication_Response& respondWith) {
        nlohmann::json statInfo = SK_File::getFileInfo(path);

        if (statInfo.contains("error")) {
            respondWith.error(404, "ENOENT");
            return;
        }

        respondWith.JSON(statInfo);;
    };
};

END_SK_NAMESPACE
