#pragma once

#include "../sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Communication {
public:
	SK_IPC_v2* sb_ipc;
	SK_Module_System* modsys;
	SK_Window_Mngr* wndMngr;

	SK_Communication() {
    #if defined(SK_OS_windows)
		SK_Global::GetInstance().onCommunicationRequest = [&](SK_Communication_Config* config, SK_Communication_handlePacket_Response_IPC_CB ipcResponseCallback, void* resHandler) {
    #elif defined(SK_OS_apple)
		SK_Global::GetInstance().onCommunicationRequest = [&](SK_Communication_Config* config, SK_Communication_handlePacket_Response_IPC_CB ipcResponseCallback, SK_Communication_AppleCB_CB resHandler) {
    #endif
			SK_Communication_Packet* packet;
			
			#if defined(SK_OS_windows)
				ICoreWebView2WebResourceRequestedEventArgs* webPayload;
			#elif defined(SK_OS_apple)
                void* webPayload;
			#elif defined(SK_OS_linux) || defined(SK_OS_android)
				//for linux and android
			#endif


			if (config->type == SK_Communication_Packet_Type::sk_comm_pt_ipc) {
				packet = packetFromIPCMessage((*static_cast<nlohmann::json*>(config->objPtr)));
			}
			else if (config->type == SK_Communication_Packet_Type::sk_comm_pt_web) {
				#if defined(SK_OS_windows)
					webPayload = static_cast<ICoreWebView2WebResourceRequestedEventArgs*>(config->objPtr);
					packet = packetFromWebRequest(webPayload, config->sender);
				#elif defined(SK_OS_apple)
                    packet = static_cast<SK_Communication_Packet*>(resHandler(nullptr));
				#elif defined(SK_OS_linux) || defined(SK_OS_android)
					//for linux and android
				#endif
			}

			packet->response()->config = config;

            packet->response()->onHandleResponse = [packet, ipcResponseCallback, webPayload, resHandler](SK_Communication_Response* response) {
                
                if (response->type == SK_Communication_Packet_Type::sk_comm_pt_ipc) {
                    ipcResponseCallback(response->getForIPC());
                }
                else if (response->type == SK_Communication_Packet_Type::sk_comm_pt_web) {
                    #if defined(SK_OS_windows)
                        webPayload->put_Response(response->getForWeb().get());
                    #elif defined(SK_OS_apple)
                    resHandler(packet);
                    #elif defined(SK_OS_linux) || defined(SK_OS_android)
                        //for linux and android
                    #endif
                }
                
				delete packet;
			};

			//Handle this packet
			handlePacket(packet);
		};
	}

	void handlePacket(SK_Communication_Packet* packet) {
		SK_Window* view = wndMngr->findWindowByTag(packet->target);

		if (view != nullptr) {
			handleForwarding(packet);
			return;
		}

        if (packet->target == "sk:test") {
            packet->response()->JSON({{"response", "OK"}});
        }
        else if (packet->target == "sk:viewIPC") {
            SK_IPC_v2* sender = getIPCForID(packet->sender);
            sender->handle_IPC_Msg(packet);
        }
		else if (packet->target == "sk:sb") {
			if (packet->info.contains("event_id")) {
				handleForwarding(packet);
			}
			else {
				#if defined SK_MODE_DEBUG
					SK_String filePath = SK_Global::GetInstance().pathUtils.paths["soft_backend"] + SK_String(packet->info["path"]);
					packet->response()->file(filePath);
				#else

				#endif
			}
		}
		else if (packet->target == "sk:modsys") {
			#if defined SK_MODE_DEBUG
				SK_String filePath = SK_Global::GetInstance().pathUtils.paths["module_system"] + SK_String(packet->info["path"]);
				packet->response()->file(filePath);
			#else

			#endif
			
		}
		else if (packet->target == "sk:modop") {
			nlohmann::json payload;

			SK_String path;

			if (!packet->data.is_null()) {
				payload = packet->data;
			}
			else {
				path = SK_String(packet->info["path"]);
				SK_String data = path.substring(2, path.length()).fromBase64();
				payload = nlohmann::json::parse(data.data);
			}

			modsys->performOperation(
				payload["module"].get<std::string>(),
				payload["operation"].get<std::string>(),
				payload["payload"],
				(*packet->response())
			);
		}
		else if (packet->target == "sk:project") {
			SK_String path = SK_String(packet->info["path"]);

			if (path.substring(0, 7) == "/sk_vfs") {
				SK_Module_vfs_file* vfile = modsys->vfs->findByPath(path);
                if (vfile){
                    packet->response()->string(vfile->data, "text/html");
                } else {
                    packet->response()->error();
                }
				return;
			}

			#if defined SK_MODE_DEBUG
				SK_String filePath = SK_Global::GetInstance().pathUtils.paths["project"] + path;
				packet->response()->file(filePath);
			#else

			#endif
		}
		else if (packet->target == "sk:view") {
			SK_String path = SK_String(packet->info["path"]);

			SK_String viewID = path.replace("/", "");

			SK_Module_vfs_file* vfile = modsys->vfs->findByPath("sk_vfs/sk_project/views/" + viewID + "/frontend/view.html");

			packet->response()->string(vfile->data, "text/html");
		}
		else if (packet->target == "sk:profiler") {
			//packet->response()->JSON(SK_Profiler::serialize());
		}
		else {
			#if defined SK_MODE_DEBUG
				std::string filePath = SK_Global::GetInstance().pathUtils.paths["project"] + SK_String(packet->info["path"]);
				packet->response()->file(filePath);
			#else

			#endif
		}
	};

	SK_IPC_v2* getIPCForID(const SK_String& id) {
		if (id == "sk:sb") {
			return sb_ipc;
		}
		else {
			SK_Window* view = wndMngr->findWindowByTag(id);
			return &view->ipc;
		}
	}

        
	void handleForwarding(SK_Communication_Packet* packet) {
		SK_IPC_v2* sender = getIPCForID(packet->sender);
		SK_IPC_v2* target = getIPCForID(packet->target);

		SK_String eventID = packet->info["event_id"];

        if (sender == nullptr) {
            int x = 0;
        }

		if (sender->eventExists(eventID) != "") {
			sender->handle_IPC_Msg(packet);	
			return;
		}
		
		if (packet->info["type"] == "response") {
			sender->handle_IPC_Msg(packet);
			return;
		}
		else {
			packet->response()->async = true;
		}


		target->request(packet->sender, packet->target, "sk.sb.forwardedPacket", packet->asIPCMessage(), [packet, sender](const SK_String& _sender, SK_Communication_Packet* responsePacket) {
			responsePacket->id = packet->id;
			sender->sendResponse(responsePacket);

			delete packet;
		});
	}








    SK_Communication_Packet* packetFromIPCMessage(const nlohmann::json& payload) {
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

    #if defined(SK_OS_windows)
        SK_Communication_Packet* packetFromWebRequest(ICoreWebView2WebResourceRequestedEventArgs* args, const SK_String& sender) {
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
                packet->info["headers"] = ExtractHeadersToJson(_headers.get());
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
        SK_Communication_Packet* packetFromWebRequest(NSURLRequest* request, const SK_String& sender) {
            SK_Communication_Packet* packet = new SK_Communication_Packet();
            packet->type = SK_Communication_Packet_Type::sk_comm_pt_web;

            packet->id = "-1";
            packet->sender = sender;

            // Full URL
            SK_String url = request.URL.absoluteString;
            SK_String path = request.URL.path;
            if (url.indexOf("sk://sk.view.") > -1) {
                if (path.length() == 1) {
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
                    NSString* bodyString = [[NSString alloc]initWithData:bodyData encoding : NSUTF8StringEncoding];
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
private:
};

END_SK_NAMESPACE
