#pragma once

#include "../sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Communication {
public:
    SK_Global* skg;

	SK_IPC_v2* sb_ipc;
	SK_Module_System* modsys;
	SK_Window_Mngr* wndMngr;

    long long int packetIdx = 0;
    std::vector<SK_Communication_Packet*> active_packets;

	SK_Communication(SK_Global* _skg) {
        skg = _skg;


        skg->deleteCommPacketWithPID = [&](const SK_String& pid) {
            remove_all_by_pid(pid);
        };

    #if defined(SK_OS_windows)
		skg->onCommunicationRequest = [&](SK_Communication_Config* config, SK_Communication_handlePacket_Response_IPC_CB ipcResponseCallback, void* resHandler) {
    #elif defined(SK_OS_apple)
		skg->onCommunicationRequest = [&](const SK_Communication_Config& config, SK_Communication_handlePacket_Response_IPC_CB ipcResponseCallback, SK_Communication_AppleCB_CB resHandler) {
    #endif
            if (skg->terminating) return; //what's the point of handling any type of communication packet if the app is terminating? (there are actually good reasons but I don't care right now)

			SK_Communication_Packet* packet;
			
			#if defined(SK_OS_windows)
				ICoreWebView2WebResourceRequestedEventArgs* webPayload;
			#elif defined(SK_OS_apple)
                void* webPayload;
			#elif defined(SK_OS_linux) || defined(SK_OS_android)
				//for linux and android
			#endif


			if (config.type == SK_Communication_Packet_Type::sk_comm_pt_ipc) {
				packet = packetFromIPCMessage((*static_cast<nlohmann::json*>(config.objPtr)));
			}
			else if (config.type == SK_Communication_Packet_Type::sk_comm_pt_web) {
				#if defined(SK_OS_windows)
					webPayload = static_cast<ICoreWebView2WebResourceRequestedEventArgs*>(config.objPtr);
					packet = packetFromWebRequest(webPayload, config.sender);
				#elif defined(SK_OS_apple)
                    packet = static_cast<SK_Communication_Packet*>(resHandler(nullptr));
				#elif defined(SK_OS_linux) || defined(SK_OS_android)
					//for linux and android
				#endif
			}

			packet->response()->config = config;
            
            #if defined(SK_BUNDLE_MODE_NONE)
                //...
            #else
                packet->response()->bundle_library = skg->bundle_library;
            #endif
            
            packet->response()->onHandleResponse = [packet, ipcResponseCallback, webPayload, resHandler](SK_Communication_Response* response) {
                
                
                if (response->type == SK_Communication_Packet_Type::sk_comm_pt_ipc) {
                    if (!response->responseless) ipcResponseCallback(response->getForIPC());
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


    ~SK_Communication() {
        if (active_packets.size() > 0) {
            int x = 0;
        }

        for (std::size_t i = active_packets.size(); i-- > 0; ) {
            auto*& pkt = active_packets[i];
            delete pkt;      // safe even if pkt == nullptr
            pkt = nullptr;   // keep slot nulled if you reuse the vector
        }

        active_packets.clear();
    }

    size_t remove_all_by_pid(const std::string& pid)
    {
        // If you need to delete the objects, do it in the predicate (side-effect).
        auto it = std::remove_if(active_packets.begin(), active_packets.end(),
            [&](SK_Communication_Packet* pkt) {
                bool match = pkt && pkt->pid == pid;
                // if (match) delete pkt; // <-- uncomment if vector OWNS them
                return match;
            });
        size_t removed = static_cast<size_t>(active_packets.end() - it);
        active_packets.erase(it, active_packets.end());
        return removed;
    }

    SK_Communication_Packet* createPacket(SK_Communication_Packet_Type type) {
        SK_String testStr = "Hello!";

        packetIdx++;
        SK_Communication_Packet* packet = new SK_Communication_Packet();
        packet->pid = SK_String(packetIdx);
        packet->type = type;

        packet->onBeforeDestroy = [&](SK_Communication_Packet* sourcePacket) {
            size_t removedCount = remove_all_by_pid(sourcePacket->pid);
            size_t count = active_packets.size();
        };


        active_packets.push_back(packet);

        return packet;
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
            if (sender) sender->handle_IPC_Msg(packet);
        }
		else if (packet->target == "sk:sb") {
			if (packet->info.contains("event_id")) {
				handleForwarding(packet);
			}
			else {
                SK_String filePath = skg->pathUtils.paths["soft_backend"] + SK_String(packet->info["path"]);
                
                #if defined(SK_BUNDLE_MODE_NONE)
                    packet->response()->file(filePath);
                #else
                    packet->response()->fileFromBundle(filePath);
                #endif
			}
		}
		else if (packet->target == "sk:modsys") {
            SK_String filePath = skg->pathUtils.paths["module_system"] + SK_String(packet->info["path"]);
            
            #if defined(SK_BUNDLE_MODE_NONE)
                packet->response()->file(filePath);
            #else
                packet->response()->fileFromBundle(filePath);
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

            SK_String filePath = skg->pathUtils.paths["project"] + path;
            
            #if defined(SK_BUNDLE_MODE_NONE)
                packet->response()->file(filePath);
            #else
                packet->response()->fileFromBundle(filePath);
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
            std::string filePath = skg->pathUtils.paths["project"] + SK_String(packet->info["path"]);
            #if defined(SK_BUNDLE_MODE_NONE)
                packet->response()->file(filePath);
            #else
                packet->response()->fileFromBundle(filePath);
            #endif
		}
	};

	SK_IPC_v2* getIPCForID(const SK_String& id) {
		if (id == "sk:sb") {
            if (!sb_ipc) return nullptr;
			return sb_ipc;
		}
		else {
			SK_Window* view = wndMngr->findWindowByTag(id);
            if (!view) return nullptr;
			return view->ipc;
		}
	}

        
	void handleForwarding(SK_Communication_Packet* packet) {
		SK_IPC_v2* sender = getIPCForID(packet->sender);
		SK_IPC_v2* target = getIPCForID(packet->target);

		SK_String eventID = packet->info["event_id"];

		if (sender->eventExists(eventID) != "") {
			sender->handle_IPC_Msg(packet);
			return;
		}
		
		if (packet->info["type"] == "response") {
			sender->handle_IPC_Msg(packet);
            delete packet;
			return;
		}
		else {
			packet->response()->async = true;
		}


		target->request(packet->sender, packet->target, "sk.sb.forwardedPacket", packet->asIPCMessage(), false, [packet, sender](const SK_String& _sender, SK_Communication_Packet* responsePacket) {
			responsePacket->id = packet->id;
			sender->sendResponse(responsePacket);

			delete packet;
		});
	}








    SK_Communication_Packet* packetFromIPCMessage(const nlohmann::json& payload) {
        SK_Communication_Packet* packet = createPacket(SK_Communication_Packet_Type::sk_comm_pt_ipc);
        packet->originalData = payload;

        packet->responseObj = new SK_Communication_Response_IPC();
        
        packet->response()->responseless = (payload.contains("responseless") && payload["responseless"] == true); 

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


            SK_Communication_Packet* packet = createPacket(SK_Communication_Packet_Type::sk_comm_pt_web);

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
            SK_Communication_Packet* packet = createPacket(SK_Communication_Packet_Type::sk_comm_pt_web);
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
            packet->info["headers"] = ExtractHeadersToJson(headers);

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
