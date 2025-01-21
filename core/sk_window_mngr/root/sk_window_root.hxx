#pragma once


#include "../../sk_common.hxx"

BEGIN_SK_NAMESPACE

class SK_Window_Root {
public:
	unsigned int wndIdx;

	SK_String tag;

	SK_String title = "SK Window";

	SK_IPC_v2 ipc;

	bool visible = false;

	int width = 800;
	int height = 600;
	int left = 0;
	int top = 0;
    
    std::optional<int> zIndex = NULL;
    bool transparent = false;
    
	SK_WebView webview;

	SK_Window_Root::SK_Window_Root() {
		
	}

	virtual void SK_Window_Root::initialize(const unsigned int& _wndIdx) {
        wndIdx = _wndIdx;
    }
private:

};

END_SK_NAMESPACE