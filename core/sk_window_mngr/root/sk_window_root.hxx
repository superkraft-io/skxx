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
		{"scale", 1.}
	};

	SK_Point maxSizeFull {-1, -1};

    
    std::optional<int> zIndex = NULL;

	bool resizing = false;

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