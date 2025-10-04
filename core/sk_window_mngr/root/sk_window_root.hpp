#pragma once


#include "../../sk_common.hpp"


BEGIN_SK_NAMESPACE

using SK_Window_Root_onDestroyed_CB = std::function<void()>;
using SK_Window_Root_windowEventMsg_CB = std::function<void(nlohmann::json data)>;
using SK_Window_Root_windowAction_CB = std::function<void(SK_Communication_Packet* packet)>;

//class SK_Window;

class SK_Window_Root {
public:
	SK_Global* skg;

    SK_Window_Root_onDestroyed_CB onDestroyed;
    SK_Window_Root_windowAction_CB onWindowAction;

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


    std::vector<SK_Window_Root*> subViews; //other SK_Window objects that also should receive window events

	SK_Point maxSizeFull {-1, -1};

    //SK_Window* parent = nullptr;
    
    std::optional<int> zIndex = NULL;

    bool __closed = false;
    
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

    bool activateMoving = false;

	SK_Color backgroundColor = "greenyellow";

    SK_WebView webview;
    
    SK_Window_Root() {
        
    }

    ~SK_Window_Root(){
        subViews.clear();
        subViews.shrink_to_fit();
    }
    
	virtual void initialize(const unsigned int& _wndIdx) {
        wndIdx = _wndIdx;

        ipc->onMessage = [&](const SK_String& sender, SK_Communication_Packet* packet) {
            SK_String action = "";
            if (packet->data.contains("action")) action = SK_String(packet->data["action"]);

            if (action == "windowAction") {
                onWindowAction(packet);
            }
        };
    }

	virtual void configWithInfo(const nlohmann::json& _info) {
		config.combineWith(_info);

        void* addr = &config_updateTracker;
        
		config_updateTracker.update(_info);
		for (auto& [key, value] : config_updateTracker.items()) {
            if (key == "resizable"){
                int x = 0;
            }
            
            config_updateTracker[key] = true;
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
        
        void* addr = &config_updateTracker;
        
        if (attribute == "resizable") {
            int x = 0;
        }
        
		bool needsUpdate = config_updateTracker[attribute];
		config_updateTracker[attribute] = false;
		return needsUpdate;
	}
    
    
    
    
    void emitWndEvent(SK_Window_Root* wnd, const SK_String& eventID, const nlohmann::json& data, SK_Window_Root_windowEventMsg_CB cb = NULL){
        if (wnd->__closed) return;
        
        nlohmann::json payload {
            {"action", "windowEvent"},
            {"windowID", wnd->tag},
            {"eventID", eventID},
            {"data", data}
        };
        

        if (skg->sb_ipc){
            SK_IPC_v2* sb_ipc = static_cast<SK_IPC_v2*>(skg->sb_ipc);
            sb_ipc->request("sk:viewIPC", "sk:sb", "sk::windowEvent::", payload, [cb](const SK_String& _sender, SK_Communication_Packet* responsePacket) {
                if (cb != NULL) cb(responsePacket->data);
            });
        }

		if (ipc) ipc->request("sk:viewIPC", tag, "sk::windowEvent", payload, [cb](const SK_String& _sender, SK_Communication_Packet* responsePacket) {
			//do nothing
		});

        for (auto* subView : subViews) {           // module is SK_Module_ProtonJS_Window
            if (subView) subView->ipc->request("sk:viewIPC", subView->tag, "sk::windowEvent::" + subView->tag, payload, [cb](const SK_String& _sender, SK_Communication_Packet* responsePacket) {
                //do nothing
            });
        }
    }

    
    void addSubView(SK_Window_Root* wnd) {
        if (!wnd) return;
        if (std::find(subViews.begin(), subViews.end(), wnd) != subViews.end()) return;
        subViews.push_back(wnd);
    }

    void removeSubView(SK_Window_Root* wnd) {
        if (!wnd) return;
        auto it = std::find(subViews.begin(), subViews.end(), wnd);
        if (it == subViews.end()) return;
        subViews.erase(it);
    }

private:

};

END_SK_NAMESPACE
