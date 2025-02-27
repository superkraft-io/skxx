#pragma once

#include "sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Window;
class SK_Communication_Packet;

using SK_Window_onWindowFocusChanged_Callback = std::function<void(SK_Window* wnd, const bool& focused)>;

using SK_WindowMngr_onFindWindowByString = std::function<SK_Window* (const SK_String& string)>;

#if defined(SK_OS_windows)
    using SK_WindowMngr_getWebview2HWNDForWindow = std::function<HWND(const SK_String& windowClassName)>;
	using SK_WindowMngr_updateWebViewHWNDListForView = std::function<void(const SK_String& windowClassName)>;
#elif defined(SK_OS_apple)
    using SK_Communication_AppleCB_CB = std::function<void*(SK_Communication_Packet* packet)>;
#elif defined(SK_OS_linux) || defined(SK_OS_android)
    //for linux and android
#endif

using SK_onMainWindowHWNDAcquired = std::function<void(void* handle)>;


	
using SK_WebView_OnReady = std::function<void(void* webview, bool isHardBackend)>;


enum SK_Communication_Packet_Type {
	sk_comm_pt_ipc = 0,
	sk_comm_pt_web = 1,
};


struct SK_Communication_Config {
	SK_String sender;
	SK_Communication_Packet_Type type;
	void* objPtr;

	#if defined(SK_OS_windows)
		wil::com_ptr<ICoreWebView2Environment> webviewEnvironment;
	#elif defined(SK_OS_apple)
		//for apple
	#elif defined(SK_OS_linux) || defined(SK_OS_android)
		//for linux and android
	#endif
};

using SK_Communication_handlePacket_Response_IPC_CB = std::function<void(const SK_String& ipcResponseData)>;
using SK_Communication_onRequest = std::function<void(SK_Communication_Config* config, SK_Communication_handlePacket_Response_IPC_CB ipcResponseCallback, SK_Communication_AppleCB_CB preparePacket)>;


using SK_ThreadPool_ProcessMainThreadTasks = std::function<void()>;

using SK_showSoftBackendDevTools = std::function<void()>;

using SK_resizeAllMainWindowView = std::function<void(int x, int y, int w, int h, float scale)>;

using SK_getMainWindowSize = std::function<SK_Point()>;
using SK_setMainWindowSize = std::function<void(int w, int h)>;

END_SK_NAMESPACE
