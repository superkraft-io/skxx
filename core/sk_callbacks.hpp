#pragma once

#include "sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Window;
class SK_Communication_Packet;
class SK_Communication_Response;


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

using SK_onMainWindowHWNDAcquired = std::function<void(void* handle, bool isView)>;


	
using SK_WebView_OnReady = std::function<void(void* wnd, void* webview, bool isHardBackend)>;
using SK_WebView_SendMsgToFrontend_CB = std::function<void(const SK_String& target, const SK_String& data)>;

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

#if defined(SK_OS_windows)
	using SK_Communication_onRequest = std::function<void(SK_Communication_Config* config, SK_Communication_handlePacket_Response_IPC_CB ipcResponseCallback, void* nullArg)>;
#elif defined(SK_OS_apple)
	using SK_Communication_onRequest = std::function<void(SK_Communication_Config* config, SK_Communication_handlePacket_Response_IPC_CB ipcResponseCallback, SK_Communication_AppleCB_CB preparePacket)>;
#endif

using SK_ThreadPool_ProcessMainThreadTasks = std::function<void()>;

using SK_showSoftBackendDevTools = std::function<void()>;

using SK_resizeAllMainWindowViews = std::function<void(int x, int y, int w, int h, float scale)>;

using SK_getMainWindowSize = std::function<SK_Point()>;
using SK_setMainWindowSize = std::function<void(int w, int h)>;



using SK_onPreConfigWnd = std::function<void(SK_Window* wnd, nlohmann::json constructorOpts)>;
using SK_onPostConfigWnd = std::function<void(SK_Window* wnd)>;
using SK_wndCreated = std::function<void(SK_Window* wnd)>;

using SK_WebView_onGetUserDataPath = std::function<SK_String(SK_Window* wnd)>;

using SK_HandlePluginParamEvent_CB = std::function<void(const nlohmann::json& payload, SK_Communication_Response& respondWith)>;

using SK_InitSK_CB = std::function<void()>;
using SK_DestroySK_CB = std::function<void()>;
using SK_OBJCPPSafeTicker_CB = std::function<void()>;
using SK_OBJCPPSafeInitializer_CB = std::function<void()>;
END_SK_NAMESPACE
