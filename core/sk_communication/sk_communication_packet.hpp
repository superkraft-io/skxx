#pragma once

#include "../sk_common.hpp"



BEGIN_SK_NAMESPACE

using SK_Communication_Packet_onBeforeDestroy_CB = std::function<void(SK_Communication_Packet*)>;

class SK_Communication_Packet {
public:
    nlohmann::json originalData;

    SK_Communication_Packet_Type type = SK_Communication_Packet_Type::sk_comm_pt_ipc;

    SK_String pid = "";
    SK_String id = "";

    SK_String sender;
    SK_String target;

    nlohmann::json info;
    nlohmann::json data;

    void* responseObj = nullptr;
    
    SK_Communication_Packet_onBeforeDestroy_CB onBeforeDestroy;

    virtual ~SK_Communication_Packet() {
        nlohmann::json().swap(originalData);
        originalData = nlohmann::json();
        originalData.clear();

        nlohmann::json().swap(info);
        info = nlohmann::json();
        info.clear();

        nlohmann::json().swap(data);
        data = nlohmann::json();
        data.clear();

        SK_Communication_Response* res = response();
        SK_Communication_Config cfg = res->config;
        
        
        if (cfg.type == SK_Communication_Packet_Type::sk_comm_pt_ipc) {
            delete static_cast<SK_Communication_Response_IPC*>(responseObj);
        }
        else if (cfg.type == SK_Communication_Packet_Type::sk_comm_pt_web) {
            delete static_cast<SK_Communication_Response_Web*>(responseObj);
        }

        responseObj = nullptr;

        onBeforeDestroy(this);
    }


    
    bool parseURLComponents(const SK_String& url, SK_Communication_Packet* packet) {
        
        SK_String pathOnly = url.replace(SK_Base_URL, "");
        
        //extract target
        SK_String targetRoute = "sk:project";
        SK_String targetPrefix = "/sk:";
        if (pathOnly.substring(0, targetPrefix.length()) == targetPrefix){
            size_t targetEndIdx = pathOnly.replace(targetPrefix, "").indexOf("/") + targetPrefix.length();
            targetRoute = pathOnly.substring(1, targetEndIdx);
            pathOnly = pathOnly.substring(targetEndIdx, pathOnly.length());
        }
        packet->target = targetRoute;
        
        
        SK_String _url = url.replace("/" + targetRoute, "");
        packet->info["url"] = _url;
        
        // Parse protocol
        size_t protocolEnd = _url.data.find("://");
        if (protocolEnd != std::string::npos) {
            packet->info["protocol"] = url.data.substr(0, protocolEnd + 3);
        }

        // Parse host and path
        size_t hostStart = protocolEnd == std::string::npos ? 0 : protocolEnd + 3;
        
        size_t pathStart = _url.data.find('/', hostStart);
        size_t queryStart = _url.data.find('?', pathStart);

        if (pathStart != std::string::npos) {
            std::string host = url.data.substr(hostStart, pathStart - hostStart);
            packet->info["host"] = host;
            packet->info["path"] = (queryStart != std::string::npos)
                ? _url.data.substr(pathStart, queryStart - pathStart)
                : _url.data.substr(pathStart);
        }
        else {
            std::string host = (queryStart != std::string::npos)
                ? _url.data.substr(hostStart, queryStart - hostStart)
                : _url.data.substr(hostStart);
            packet->info["host"] = host;
            packet->info["path"] = "/";
        }

        // Parse query parameters
        if (queryStart != std::string::npos) {
            size_t paramStart = queryStart + 1;
            while (paramStart < _url.data.size()) {
                size_t eqPos = _url.data.find('=', paramStart);
                size_t ampPos = _url.data.find('&', paramStart);

                if (eqPos == std::string::npos) break;

                std::string key = _url.data.substr(paramStart, eqPos - paramStart);
                std::string value = (ampPos != std::string::npos)
                    ? _url.data.substr(eqPos + 1, ampPos - eqPos - 1)
                    : _url.data.substr(eqPos + 1);

                packet->info["parameters"][key] = value;

                if (ampPos == std::string::npos) break;
                paramStart = ampPos + 1;
            }
        }
        
        return true;
    }
    
    



    SK_Communication_Response* response() {
        return static_cast<SK_Communication_Response*>(responseObj);
    }

    SK_String dump(int indents = -1) {
        nlohmann::json json = {
            {"id", id},
            {"sender", sender},
            {"target", target},
            {"info", info},
            {"type", info.contains("type") ? info["type"] : nlohmann::json()},
            {"data", data}
        };

        return json.dump(indents);
    }

    SK_String asIPCMessage(int indents = -1) {
        nlohmann::json json = {
            {"msg_id", id},
            {"sender", sender},
            {"target", target},
            {"type", info.contains("type") ? info["type"] : nlohmann::json()},
            {"data", data}
        };
        json.update(info);

        return json.dump(indents);
    }

private:
};

END_SK_NAMESPACE
