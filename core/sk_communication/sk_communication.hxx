#pragma once

#include "../sk_common.hxx"

BEGIN_SK_NAMESPACE




class SK_Communication {
public:
	SK_IPC_v2* sb_ipc;
	SK_Module_System* modsys;
	SK_Window_Mngr* wndMngr;

	SK_Communication::SK_Communication() {
		int x = 0;
		SK_Common::onCommunicationRequest = [&](SK_Communication_Config* config, SK_Communication_handlePacket_Response_IPC_CB ipcResponseCallback) {
			SK_Communication_Packet* packet;
			
			#if defined(SK_OS_windows)
				ICoreWebView2WebResourceRequestedEventArgs* webPayload;
			#elif defined(SK_OS_macos) || defined(SK_OS_ios)
						//for apple
			#elif defined(SK_OS_linux) || defined(SK_OS_android)
						//for linux and android
			#endif


			if (config->type == SK_Communication_Packet_Type::sk_comm_pt_ipc) {
				packet = SK_Communication_Packet::packetFromIPCMessage((*static_cast<nlohmann::json*>(config->objPtr)));
			}
			else if (config->type == SK_Communication_Packet_Type::sk_comm_pt_web) {
				#if defined(SK_OS_windows)
					webPayload = static_cast<ICoreWebView2WebResourceRequestedEventArgs*>(config->objPtr);
					packet = SK_Communication_Packet::packetFromWebRequest(webPayload, config->sender);
				#elif defined(SK_OS_macos) || defined(SK_OS_ios)
					//for apple
				#elif defined(SK_OS_linux) || defined(SK_OS_android)
					//for linux and android
				#endif
			}

			packet->response()->config = config;

			packet->response()->onHandleResponse = [packet, ipcResponseCallback, webPayload](SK_Communication_Response* response) {
				if (response->type == SK_Communication_Packet_Type::sk_comm_pt_ipc) {
					ipcResponseCallback(response->getForIPC());
				}
				else if (response->type == SK_Communication_Packet_Type::sk_comm_pt_web) {
					webPayload->put_Response(response->getForWeb().get());
				}

				delete packet;
			};

			//Handle this packet
			handlePacket(packet);
		};
	}

	void handlePacket(SK_Communication_Packet* packet) {
		SK_Window* view = wndMngr->findByTag(packet->target);

		if (view != nullptr) {
			handleForwarding(packet);
			return;
		}

		if (packet->target == "sk.sb") {
			if (packet->info.contains("event_id")) {
				handleForwarding(packet);
			}
			else {
				#if defined SK_MODE_DEBUG
					SK_String filePath = SK::SK_Path_Utils::paths["soft_backend"] + SK_String(packet->info["path"]);
					packet->response()->file(filePath);
				#else

				#endif
			}
		}
		else if (packet->target == "sk.modsys") {
			#if defined SK_MODE_DEBUG
				SK_String filePath = SK::SK_Path_Utils::paths["module_system"] + SK_String(packet->info["path"]);
				packet->response()->file(filePath);
			#else

			#endif
			
		}
		else if (packet->target == "sk.modop") {
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
		else if (packet->target == "sk.project") {
			int x = 0;

			SK_String path = SK_String(packet->info["path"]);

			if (path.substring(0, 7) == "/sk_vfs") {
				SK_Module_vfs_file* vfile = modsys->vfs->findByPath(path);
				packet->response()->string(vfile->data, "text/html");
				return;
			}

			#if defined SK_MODE_DEBUG
				SK_String filePath = SK::SK_Path_Utils::paths["project"] + path;
				packet->response()->file(filePath);
			#else

			#endif
		}
		else if (packet->target == "sk.view") {
			int x = 0;

			SK_String path = SK_String(packet->info["path"]);

			SK_String viewID = path.replace("/", "");

			SK_Module_vfs_file* vfile = modsys->vfs->findByPath("sk_vfs/project/views/" + viewID + "/frontend/view.html");

			packet->response()->string(vfile->data, "text/html");
		}
		else if (packet->target == "sk.profiler") {
			packet->response()->JSON(SK_Profiler::serialize());
		}
		else {
			int x = 0;
			#if defined SK_MODE_DEBUG
				std::string filePath = SK::SK_Path_Utils::paths["project"] + SK_String(packet->info["path"]);
				packet->response()->file(filePath);
			#else

			#endif
		}
	};

	SK_IPC_v2* getIPCForID(const SK_String& id) {
		if (id == "sk.sb") {
			return sb_ipc;
		}
		else {
			SK_Window* view = wndMngr->findByTag(id);
			return &view->ipc;
		}
	}

	void handleForwarding(SK_Communication_Packet* packet) {
		SK_IPC_v2* sender = getIPCForID(packet->sender);
		SK_IPC_v2* target = getIPCForID(packet->target);

		if (sender->eventExists(SK_String(packet->info["event_id"])) != "") {
			sender->handle_IPC_Msg(packet);
			return;
		}
		
		if(packet->info["type"] == "response") {
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
private:
};

END_SK_NAMESPACE
