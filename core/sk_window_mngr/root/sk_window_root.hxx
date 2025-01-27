#pragma once


#include "../../sk_common.hxx"

BEGIN_SK_NAMESPACE

class SK_Window_Root {
public:
	unsigned int wndIdx;
	SK_String tag;
	SK_String title = "SK Window";
	SK_IPC_v2 ipc;

	SK_JSON config {
		{"mainWindow", false}, //ok

		{"visible", true}, //ok

		{"scale", 1.},

		{"left", 0},
		{"top", 0},
		{"width", 800},    //ok
		{"height", 600},   //ok
		{"minWidth", -1},  //ok
		{"minHeight", -1}, //ok
		{"maxWidth", -1},  //ok
		{"maxHeight", -1}, //ok

		{"alwaysOnTop", false},
		{"skipTaskbar", false},

		{"alpha", 1.},

		{"frame", true},
		{"transparent", false}

	};

	SK_Point maxSizeFull {-1, -1};

    
    std::optional<int> zIndex = NULL;

	bool frameless_drag;
	bool frameless_resize;

	SK_Color backgroundColor = "black";

	SK_WebView webview;

	SK_Window_Root::SK_Window_Root() {
		
	}

	virtual void SK_Window_Root::initialize(const unsigned int& _wndIdx) {
        wndIdx = _wndIdx;
    }

	virtual void configWithInfo(const nlohmann::json& _info) {
		config.combineWith(_info);

	}
private:

};

END_SK_NAMESPACE