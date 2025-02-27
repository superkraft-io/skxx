#pragma once

#include "../sk_common.hpp"



BEGIN_SK_NAMESPACE

class SK_Communication_Packet {
public:
    nlohmann::json originalData;

    SK_Communication_Packet_Type type = SK_Communication_Packet_Type::sk_comm_pt_ipc;

    SK_String id;

    SK_String sender;
    SK_String target;

    nlohmann::json info;
    nlohmann::json data;

    void* responseObj;
        

    ~SK_Communication_Packet() {
        if (response()->config->type == SK_Communication_Packet_Type::sk_comm_pt_ipc) {
            delete static_cast<SK_Communication_Response_IPC*>(responseObj);
        }
        else if (response()->config->type == SK_Communication_Packet_Type::sk_comm_pt_web) {
            //delete static_cast<SK_Communication_Response_Web*>(responseObj);
        }
    }

    static inline SK_Communication_Packet* packetFromIPCMessage(const nlohmann::json& payload) {
        SK_Communication_Packet* packet = new SK_Communication_Packet();
        packet->originalData = payload;

        packet->responseObj = new SK_Communication_Response_IPC();

        packet->response()->packageIPCResponse = [&, packet](const nlohmann::json& data) -> SK_String {
            nlohmann::json results{
                {"sender", packet->target},
                {"target", packet->sender},
                {"msg_id", packet->id},
                {"event_id", packet->info["event_id"]},
                {"type", "response"},
                {"data", data}
            };

            SK_String dumpStr = results.dump();
            return dumpStr;
        };

        SK_String msg_id = payload["msg_id"];
        SK_String sender = payload["sender"];
        SK_String target = payload["target"];

        packet->id = msg_id;
        packet->sender = sender;
        packet->target = target;

        packet->info["type"] = payload["type"];
        packet->info["event_id"] = payload["event_id"];

        packet->data = payload["data"];

        return packet;
    };

    
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
        
        int x = 0;
        
        
        
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
    
    
#if defined(SK_OS_windows)
    static inline SK_Communication_Packet* packetFromWebRequest(ICoreWebView2WebResourceRequestedEventArgs* args, const SK_String& sender) {
        wil::com_ptr<ICoreWebView2WebResourceRequest> request;
        args->get_Request(&request);

        SK_Communication_Packet* packet = new SK_Communication_Packet();
        packet->type = SK_Communication_Packet_Type::sk_comm_pt_web;

        packet->id = "-1";
        packet->sender = sender;

        wil::unique_cotaskmem_string _url;
        if (SUCCEEDED(request->get_Uri(&_url))) {
            packet->info["url"] = wstringToString(_url.get());
            packet->parseURLComponents(wstringToString(_url.get()), packet);
        }

        packet->responseObj = new SK_Communication_Response_Web(SK_String(packet->info["url"]));

        wil::unique_cotaskmem_string _method;
        if (SUCCEEDED(request->get_Method(&_method))) {
            packet->info["method"] = wstringToString(_method.get());
        }

        wil::com_ptr<ICoreWebView2HttpRequestHeaders> _headers;
        if (SUCCEEDED(request->get_Headers(&_headers))) {
            packet->info["headers"] = packet->ExtractHeadersToJson(_headers.get());
        }

        return packet;
    };

    

    nlohmann::json ExtractHeadersToJson(ICoreWebView2HttpRequestHeaders* headers) {
        nlohmann::json headersJson;

        wil::com_ptr<ICoreWebView2HttpHeadersCollectionIterator> iterator;
        if (SUCCEEDED(headers->GetIterator(&iterator))) {
            while (true) {
                wil::unique_cotaskmem_string headerName, headerValue;
                HRESULT hr = iterator->GetCurrentHeader(&headerName, &headerValue);

                if (SUCCEEDED(hr)) {
                    headersJson[wstringToString(headerName.get())] = wstringToString(headerValue.get());
                }
                else {
                    break;
                }

                BOOL hasNext = FALSE;
                if (FAILED(iterator->MoveNext(&hasNext)) || !hasNext) {
                    break;
                }
            }
        }

        return headersJson;
    }

#elif defined(SK_OS_apple)
    #ifdef __OBJC__
        static inline SK_Communication_Packet* packetFromWebRequest(NSURLRequest* request, const SK_String& sender) {
            SK_Communication_Packet* packet = new SK_Communication_Packet();
            packet->type = SK_Communication_Packet_Type::sk_comm_pt_web;

            packet->id = "-1";
            packet->sender = sender;

            // Full URL
            SK_String url = request.URL.absoluteString;
            SK_String path = request.URL.path;
            if (url.indexOf("sk://sk.view.") > -1){
                if (path.length() == 1){
                    SK_String viewID = url.replace("sk://sk.view.", "").replace("/", "");
                    url = SK_Base_URL + "/sk_vfs/sk_project/views/" + viewID + "/frontend/view.html";
                }
            }
            
            packet->parseURLComponents(url, packet);
            
            packet->responseObj = new SK_Communication_Response_Web(packet->info["url"]);

            // HTTP Method (GET, POST, etc.)
            packet->info["method"] = SK_String(request.HTTPMethod);

            // Request Headers
            NSDictionary<NSString*, NSString*>* headers = request.allHTTPHeaderFields;
            packet->info["headers"] = packet->ExtractHeadersToJson(headers);

            // Request Body (if POST)
            if (packet->info["method"] == "POST") {
                NSData* bodyData = request.HTTPBody;
                if (bodyData) {
                    NSString* bodyString = [[NSString alloc] initWithData:bodyData encoding:NSUTF8StringEncoding];
                    packet->info["body"] = SK_String(bodyString);
                }
            }

            return packet;
        };

        nlohmann::json ExtractHeadersToJson(NSDictionary<NSString*, NSString*>* headers) {
            nlohmann::json headersJson;

            for (NSString* key in headers) {
                headersJson[SK_String(key)] = SK_String(headers[key]);
            }

            return headersJson;
        }
    #endif
#elif defined(SK_OS_linux) || defined(SK_OS_android)
    // For Linux and Android
#endif

    SK_Communication_Response* response() {
        return static_cast<SK_Communication_Response*>(responseObj);
    }

    SK_String dump(int indents = -1) {
        nlohmann::json json = {
            {"id", id},
            {"sender", sender},
            {"target", target},
            {"info", info},
            {"type", info},
            {"data", data}
        };

        return json.dump(indents);
    }

    SK_String asIPCMessage(int indents = -1) {
        nlohmann::json json = {
            {"msg_id", id},
            {"sender", sender},
            {"target", target},
            {"data", data}
        };
        json.update(info);

        return json.dump(indents);
    }

private:
};

END_SK_NAMESPACE
