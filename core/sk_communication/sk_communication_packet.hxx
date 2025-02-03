#pragma once

#include "../sk_common.hxx"

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

	//SK_Profiler_Event* pEvent;

	~SK_Communication_Packet() {
        if (response()->config->type == SK_Communication_Packet_Type::sk_comm_pt_ipc) {
            delete static_cast<SK_Communication_Response_IPC*>(responseObj);
        }
        else if (response()->config->type == SK_Communication_Packet_Type::sk_comm_pt_web) {
            //delete static_cast<SK_Communication_Response_Web*>(responseObj);
        }
		//if (pEvent != NULL) pEvent->stop();
	}

	static inline SK_Communication_Packet* packetFromIPCMessage(const nlohmann::json& payload) {
		SK_Communication_Packet* packet = new SK_Communication_Packet();
		//packet->pEvent = SK_Profiler::snap("", "packetFromIPCMessage", NULL);

		packet->originalData = payload;

		packet->responseObj = new SK_Communication_Response_IPC();

		packet->response()->packageIPCResponse = [&, packet](const nlohmann::json& data) -> SK_String {
			nlohmann::json results{
				{"sender", packet->target},
				{"target", packet->sender},
				{"msg_id", packet->id},
				{"event_id",packet->info["event_id"]},
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


#if defined(SK_OS_windows)
	static inline SK_Communication_Packet* packetFromWebRequest(ICoreWebView2WebResourceRequestedEventArgs* args, const SK_String& sender) {
		

		wil::com_ptr<ICoreWebView2WebResourceRequest> request;
		args->get_Request(&request);

		SK_Communication_Packet* packet = new SK_Communication_Packet();
		//packet->pEvent = SK_Profiler::snap("", "packetFromWebRequest", NULL);

		packet->type = SK_Communication_Packet_Type::sk_comm_pt_web;

		packet->id = "-1";
		packet->sender = sender;

		wil::unique_cotaskmem_string _url;
		if (SUCCEEDED(request->get_Uri(&_url))) {
			packet->info["url"] = wstringToString(_url.get()); //this is based on the host in the URL, e.g "sk.be"
			packet->parseURLParameters_v2(wstringToString(_url.get()), packet);
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


	bool parseURLParameters_v2(const SK_String& url, SK_Communication_Packet* packet) {
		// Parse protocol
		size_t protocolEnd = url.data.find("://");
		if (protocolEnd != std::string::npos) {
			packet->info["protocol"] = url.data.substr(0, protocolEnd + 3); // Include "://"
		}

		// Parse host and path
		size_t hostStart = protocolEnd == std::string::npos ? 0 : protocolEnd + 3;
		size_t pathStart = url.data.find('/', hostStart);
		size_t queryStart = url.data.find('?', pathStart);

		if (pathStart != std::string::npos) {
			std::string host = url.data.substr(hostStart, pathStart - hostStart);
			packet->info["host"] = host;
			packet->target = host;
			packet->info["path"] = (queryStart != std::string::npos)
				? url.data.substr(pathStart, queryStart - pathStart)
				: url.data.substr(pathStart);
		}
		else {
			std::string host = (queryStart != std::string::npos)
				? url.data.substr(hostStart, queryStart - hostStart)
				: url.data.substr(hostStart);
			packet->info["host"] = host;
			packet->target = host;
			packet->info["path"] = "/";
		}

		// Parse query parameters
		if (queryStart != std::string::npos) {
			size_t paramStart = queryStart + 1; // Skip '?'
			while (paramStart < url.data.size()) {
				size_t eqPos = url.data.find('=', paramStart);
				size_t ampPos = url.data.find('&', paramStart);

				if (eqPos == std::string::npos) break; // No more parameters

				std::string key = url.data.substr(paramStart, eqPos - paramStart);
				std::string value = (ampPos != std::string::npos)
					? url.data.substr(eqPos + 1, ampPos - eqPos - 1)
					: url.data.substr(eqPos + 1);

				packet->info["parameters"][key] = value;

				if (ampPos == std::string::npos) break; // No more '&'
				paramStart = ampPos + 1;
			}
		}

		return true;
	}

	nlohmann::json ExtractHeadersToJson(ICoreWebView2HttpRequestHeaders* headers) {
		nlohmann::json headersJson;

		// Start by getting the iterator for the headers
		wil::com_ptr<ICoreWebView2HttpHeadersCollectionIterator> iterator;
		if (SUCCEEDED(headers->GetIterator(&iterator))) {
			while (true) {
				// Get the current header's name and value
				wil::unique_cotaskmem_string headerName, headerValue;
				HRESULT hr = iterator->GetCurrentHeader(&headerName, &headerValue);

				if (SUCCEEDED(hr)) {
					// Add the header name and value to the JSON object
					headersJson[wstringToString(headerName.get())] = wstringToString(headerValue.get());
				}
				else {
					break; // Stop if GetCurrentHeader fails
				}


				// Move to the next header; stop if no more headers
				BOOL hasNext = FALSE;
				if (FAILED(iterator->MoveNext(&hasNext)) || !hasNext) {
					break;
				}
			}
		}

		return headersJson;
	}

#elif defined(SK_OS_macos) || defined(SK_OS_ios)
    static inline SK_Communication_Packet* packetFromWebRequest(WKURLSchemeTask* request, const SK_String& sender) {
        SK_Communication_Packet* packet = new SK_Communication_Packet();
        //packet->pEvent = SK_Profiler::snap("", "packetFromWebRequest", NULL);


        packet->type = SK_Communication_Packet_Type::sk_comm_pt_web;

        packet->id = "-1";
        packet->sender = sender;

        
        // Full URL
        packet->info["url"] = SK_String(request.URL.absoluteString);
        packet->parseURLParameters_v2(SK_String(request.URL.absoluteString), packet);
        
        packet->responseObj = new SK_Communication_Response_Web(SK_String(request.URL.absoluteString));
        
        // HTTP Method (GET, POST, etc.)
        packet->info["method"] = SK_String(request.HTTPMethod);

        // Request Headers
        packet->info["headers"] = packet->ExtractHeadersToJson(request.allHTTPHeaderFields);

        // Request Body (if POST)
        if (packet->info["method"] == "POST") {
            NSData *bodyData = request.HTTPBody;
            if (bodyData) {
                NSString *bodyString = [[NSString alloc] initWithData:bodyData encoding:NSUTF8StringEncoding];
                NSLog(@"Request Body: %@", bodyString);
            } else {
                NSLog(@"No body in the POST request.");
            }
        }

        return packet;
    };


    bool parseURLParameters_v2(const SK_String& url, SK_Communication_Packet* packet) {
        // Parse protocol
        size_t protocolEnd = url.data.find("://");
        if (protocolEnd != std::string::npos) {
            packet->info["protocol"] = url.data.substr(0, protocolEnd + 3); // Include "://"
        }

        // Parse host and path
        size_t hostStart = protocolEnd == std::string::npos ? 0 : protocolEnd + 3;
        size_t pathStart = url.data.find('/', hostStart);
        size_t queryStart = url.data.find('?', pathStart);

        if (pathStart != std::string::npos) {
            std::string host = url.data.substr(hostStart, pathStart - hostStart);
            packet->info["host"] = host;
            packet->target = host;
            packet->info["path"] = (queryStart != std::string::npos)
                ? url.data.substr(pathStart, queryStart - pathStart)
                : url.data.substr(pathStart);
        }
        else {
            std::string host = (queryStart != std::string::npos)
                ? url.data.substr(hostStart, queryStart - hostStart)
                : url.data.substr(hostStart);
            packet->info["host"] = host;
            packet->target = host;
            packet->info["path"] = "/";
        }

        // Parse query parameters
        if (queryStart != std::string::npos) {
            size_t paramStart = queryStart + 1; // Skip '?'
            while (paramStart < url.data.size()) {
                size_t eqPos = url.data.find('=', paramStart);
                size_t ampPos = url.data.find('&', paramStart);

                if (eqPos == std::string::npos) break; // No more parameters

                std::string key = url.data.substr(paramStart, eqPos - paramStart);
                std::string value = (ampPos != std::string::npos)
                    ? url.data.substr(eqPos + 1, ampPos - eqPos - 1)
                    : url.data.substr(eqPos + 1);

                packet->info["parameters"][key] = value;

                if (ampPos == std::string::npos) break; // No more '&'
                paramStart = ampPos + 1;
            }
        }

        return true;
    }

    nlohmann::json ExtractHeadersToJson(NSDictionary* headers) {
        nlohmann::json headersJson;

        for (NSString *key in headers) {
            NSString *value = headers[key];
            headersJson[SK_String(key)] = SK_String(value);
        }
            
        return headersJson;
    }
#elif defined(SK_OS_linux) || defined(SK_OS_android)
	//for linux and android
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
			//{"originalData", originalData}
		};
		json.update(info);

		return json.dump(indents);
	}
private:
};

END_SK_NAMESPACE
