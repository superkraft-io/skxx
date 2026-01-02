#pragma once

#include "../../../../core/sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Module_fs {
public:
    SK_Global* skg;

    SK_Module_fs(SK_Global* _skg) {
        skg = _skg;
    }
    
    ~SK_Module_fs(){
        skg = nullptr;
    }

    void handleOperation(const SK_String& operation, nlohmann::json& payload, SK_Communication_Response& respondWith) {
        SK_String _path = payload["path"];
        if (_path.length() == 0) payload["path"] = "/";
        
        
        SK_String path = SK_String(std::filesystem::path(_path).lexically_normal().string()).replaceAll("\\", "/");
        if (path.length() > 1 && path.substring(path.length() - 1, 1) == "/") path = path.substring(0, path.length() - 1);
        
        /*if (path.indexOf("sk:modsys") > -1) {
            path = path.replace("sk:modsys", "");
            payload["path"] = path.data;
        }*/
        
        
        //If path starts with sk_vfs/, we route the operation to the VFS module
        if (path.indexOf("sk_vfs/") > -1) {
            skg->forwardPacketToModule("vfs", operation, payload, respondWith);
            return;
        }
        
       

        bool bypassBDFS = false;

        if (path.indexOf("ph_fs/") > -1) {
            bypassBDFS = true;
            path = path.replace("ph_fs/", "");
        }


        SK_String fullPath = path;

        SK_Path_Utils* pathUtils = &skg->pathUtils;

        //If the path is not absolute, then make the soft_backend folder the root folder
        if (path.substring(0, 1) == "/" || !SK_File::isPathAbsolute(path)) {
            if (operation != "mkdir") {
                SK_String targetPrefix = path.substring(0, path.indexOf("/"));
                if (targetPrefix == "sk:modsys") {
                    fullPath = pathUtils->paths["module_system"] + path.replace("sk:modsys", "");
                }
                else {
                    fullPath = pathUtils->paths["soft_backend"] + path;
                }
            }
        }

        if (!bypassBDFS){
            if (!SK_File::isPathAbsolute(path)) {
                //!!! IMPORTANT !!!! If in RELEASE mode, we route the operation to the BDFS module
                #if defined(SK_ROUTE_FS_TO_BDFS)// || defined(SK_BUNDLE_MODE_DEEP) || defined(SK_BUNDLE_MODE_SHALLOW)
                    skg->forwardPacketToModule("bdfs", operation, payload, respondWith);
                    return;
                #endif
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
        
        if (!SK_File::exists(path)) {
            respondWith.error(404, "ENOENT");
            return;
        }

        respondWith.JSON({});
    };

    void writeFile(const SK_String& path, const SK_String& data, SK_Communication_Response& respondWith) {
        if (SK_File::exists(path)) SK_File::unlink(path);

        SK_File file;
        file.data = data;

        bool writeStatus = file.saveToDisk(path);

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
        
        std::sort(fileList.begin(), fileList.end(), [](const nlohmann::json& a, const nlohmann::json& b) {
            return a["name"] < b["name"];
        });

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
