#pragma once


#include "../../sk_common.hxx"

BEGIN_SK_NAMESPACE

class SK_Window_Root {
public:
	unsigned int wndIdx;
	SK_String tag;
	SK_IPC_v2 ipc;

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

    
    std::optional<int> zIndex = NULL;

	bool resizing = false;
	bool isMaximized = false;
	bool frameless_drag;
	bool frameless_resize;

	SK_Color backgroundColor = "black";

	SK_WebView webview;

	SK_Window_Root() {
		
	}

	virtual void initialize(const unsigned int& _wndIdx) {
        wndIdx = _wndIdx;
    }

	virtual void configWithInfo(const nlohmann::json& _info) {
		config.combineWith(_info);

		config_updateTracker.update(_info);
		for (auto& [key, value] : config_updateTracker.items()) {
			value = true; // Set to boolean true
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
private:

};

END_SK_NAMESPACE
