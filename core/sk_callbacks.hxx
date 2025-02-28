#pragma once

#include "sk_common.hxx"

BEGIN_SK_NAMESPACE

class SK_Window;

using SK_Window_onWindowFocusChanged_Callback = std::function<void(SK_Window* wnd, const bool& focused)>;

using SK_WindowMngr_onFindByWindowClassName = std::function<SK_Window* (const SK_String& windowClassName)>;
using SK_WindowMngr_getWebview2HWNDForWindow = std::function<HWND(const SK_String& windowClassName)>;
#if defined(SK_OS_windows)
	using SK_WindowMngr_updateWebViewHWNDListForView = std::function<void(const SK_String& windowClassName)>;
	using SK_onMainWindowHWNDAcquired = std::function<void(HWND hwnd)>;
#endif

	
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
	#elif defined(SK_OS_macos) || defined(SK_OS_ios)
		//for apple
	#elif defined(SK_OS_linux) || defined(SK_OS_android)
		//for linux and android
	#endif
};

using SK_Communication_handlePacket_Response_IPC_CB = std::function<void(const SK_String& ipcResponseData)>;
using SK_Communication_onRequest = std::function<void(SK_Communication_Config* config, SK_Communication_handlePacket_Response_IPC_CB ipcResponseCallback)>;


using SK_ThreadPool_ProcessMainThreadTasks = std::function<void()>;

END_SK_NAMESPACE