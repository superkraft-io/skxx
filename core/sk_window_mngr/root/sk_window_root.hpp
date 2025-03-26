#pragma once


#include "../../sk_common.hpp"


BEGIN_SK_NAMESPACE


using SK_Window_Root_windowEventMsg_CB = std::function<void(nlohmann::json data)>;

class SK_Window;

class SK_Window_Root {
public:
	SK_Global* skg;

	unsigned int wndIdx;
    SK_String windowClassName = "SK_Window";
	SK_String tag;
	SK_IPC_v2* ipc = new SK_IPC_v2();

	SK_JSON_Callback config {
		{"mainWindow", false},
		{"scale", 1.},
		{"title", "SK++ Window"}
	};

	nlohmann::json config_updateTracker {
		{"mainWindow", true},
		{"scale", true},
		{ "title", true}
	};

	SK_Point maxSizeFull {-1, -1};

    SK_Window* parent;
    
    std::optional<int> zIndex = NULL;

    
	bool isReady = false;
    bool isClosed = false;
    
	bool resizing = false;
	bool isMaximized = false;
	bool isMinimized = false;

	bool isMoving = false;
	bool isResizing = false;


	bool frameless_drag = false;
    bool frameless_resize = false;

    bool shouldClose = true;
	bool shouldClose_2ndPass = false;

	SK_Color backgroundColor = "greenyellow";

    SK_WebView webview;


	virtual void initialize(const unsigned int& _wndIdx) {
        wndIdx = _wndIdx;
    }

	virtual void configWithInfo(const nlohmann::json& _info) {
		config.combineWith(_info);

		config_updateTracker.update(_info);
		for (auto& [key, value] : config_updateTracker.items()) {
			value = true;
		}

	}

	bool needsWindowUpdate() {
		bool value = false;

		if (config_updateTracker["backgroundColor"]) value = true;

		return value;
	}

	bool needsUpdate(const SK_String& attribute) {
		return config_updateTracker[attribute];
	}

	bool checkNeedsUpdateAndReset(const SK_String& attribute) {
		bool needsUpdate = config_updateTracker[attribute];
		config_updateTracker[attribute] = false;
		return needsUpdate;
	}
    
    
    
    
    void emitWndEvent(SK_Window_Root* wnd, const SK_String& eventID, const nlohmann::json& data, SK_Window_Root_windowEventMsg_CB cb = NULL){
        nlohmann::json payload {
            {"action", "windowEvent"},
            {"windowID", wnd->tag},
            {"eventID", eventID},
            {"data", data}
        };
        

		SK_IPC_v2* sb_ipc = static_cast<SK_IPC_v2*>(skg->sb_ipc);
		sb_ipc->request("sk:viewIPC", "sk:sb", "sk::windowEvent::" + wnd->tag, payload, [cb](const SK_String& _sender, SK_Communication_Packet* responsePacket) {
            if (cb != NULL) cb(responsePacket->data);
        });

		ipc->request("sk:viewIPC", tag, "sk::windowEvent", payload, [cb](const SK_String& _sender, SK_Communication_Packet* responsePacket) {
			//do nothing
		});
    }
private:

};

END_SK_NAMESPACE
