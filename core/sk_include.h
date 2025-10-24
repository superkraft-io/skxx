#pragma once

#include "sk_include_core.h"


#include "utils/sk_string.h"
#include "utils/sk_number.hpp"
#include "utils/sk_path_utils.hpp"
#include "utils/sk_array.hpp"
#include "utils/sk_datetime.hpp"
#include "utils/sk_str_utils.hpp"
#include "utils/sk_color.hpp"

#include "utils/sk_logger.h"

#include "sk_json/sk_json_callback.hpp"

#include "utils/sk_point.hpp"


#include "sk_web/sk_web_utils.hpp"
#include "utils/sk_file.hpp"



#include "sk_callbacks.hpp"

#include "sk_threads/sk_thread_pool.hpp"

#include "sk_timer/sk_timer.h"
#include "utils/sk_displayUtils/sk_displayUtils.h"


#include "../libs/general/curl/curl.h"
#include "sk_web/sk_curl.hpp"


#if defined(SK_BUNDLE_MODE_DEEP) || defined(SK_BUNDLE_MODE_SHALLOW)
    #include "../../../../.sk/bundle/sk_soft_backend_bundle_library.h"
#endif


#include "sk_communication/sk_communication_response.hpp"
#include "sk_communication/sk_communication_packet.hpp"



BEGIN_SK_NAMESPACE

class SK_Global {
public:
    bool terminating = false;

    SK_String newUUID() {
        #if defined(SK_OS_windows)
            UUID uuid;
            if (CoCreateGuid(&uuid) != S_OK) {
                return "´<newUUID() error>";
            }

            std::stringstream ss;
            ss << std::hex << std::setfill('0')
                << std::setw(8) << uuid.Data1 << "-"
                << std::setw(4) << uuid.Data2 << "-"
                << std::setw(4) << uuid.Data3 << "-"
                << std::setw(2) << static_cast<int>(uuid.Data4[0])
                << std::setw(2) << static_cast<int>(uuid.Data4[1]) << "-"
                << std::setw(2) << static_cast<int>(uuid.Data4[2])
                << std::setw(2) << static_cast<int>(uuid.Data4[3])
                << std::setw(2) << static_cast<int>(uuid.Data4[4])
                << std::setw(2) << static_cast<int>(uuid.Data4[5])
                << std::setw(2) << static_cast<int>(uuid.Data4[6])
                << std::setw(2) << static_cast<int>(uuid.Data4[7]);

            return ss.str();
        #endif
        
        return "";
    }

    SK_TimerMngr* timerMngr;
    SK_Timer* syncTimer;

    SK_Path_Utils pathUtils;
    void* machine;

    nlohmann::json sk_config;
    
    long long ipc_msg_id = 0;

	SK_String runningAs = "unknown";

    void* framework_base;
    
    
    #if defined(SK_BUNDLE_MODE_DEEP) || defined(SK_BUNDLE_MODE_SHALLOW)
        SK_SoftBackend_Bundle_Library* bundle_library;
    #endif

    
    void* sk = nullptr;
    
    SK_ForwardPacketToModule_CB forwardPacketToModule;


	SK_Window_onWindowFocusChanged_Callback onWindowFocusChanged;

	SK_WindowMngr_onFindWindowByString onFindWindowByClassName;
	SK_WindowMngr_onFindWindowByString onFindWindowByTag;
	
    SK_WebView_SendMsgToFrontend_CB sendMsgToWebview;
	SK_WebView_OnReady onWebViewReady;

	SK_Communication_onRequest onCommunicationRequest;

    SK_deleteCommPacketWithPID_CB deleteCommPacketWithPID;
    
    
    SK_onMainWindowHWNDAcquired onMainWindowHWNDAcquired;
    

    void* sb_ipc;

    SK_Window* mainWindow;

    #if defined(SK_OS_windows)
        //HWND mainWindowHandle;
        SK_WindowMngr_updateWebViewHWNDListForView updateWebViewHWNDListForView;
        SK_WindowMngr_getWebview2HWNDForWindow getWebview2HWNDForWindow;
    #elif defined(SK_OS_macos)
        //void* mainWindowHandle;
    #endif

    void* appInitializer;

	SK_Thread_Pool* threadPool = new SK_Thread_Pool(8);

	SK_ThreadPool_ProcessMainThreadTasks threadPool_processMainThreadTasks = [&]() {
        threadPool->processMainThreadTasks();
	};

	SK_showSoftBackendDevTools showSoftBackendDevTools;

	SK_resizeAllMainWindowViews resizeAllMainWindowViews;

	SK_getMainWindowSize getMainWindowSize;
	SK_setMainWindowSize setMainWindowSize;

    SK_onPreConfigWnd onPreConfigWnd = NULL;
    SK_onPostConfigWnd onPostConfigWnd = NULL;
    SK_wndCreated onWndCreated = NULL;
    SK_onBeforeWndResize_CB onBeforeWndResize = NULL;
    
    SK_WebView_onGetUserDataPath onGetWebViewUserDataPath;
    
    SK_HandlePluginParamEvent_CB handlePluginParamEvent;
    SK_GetPluginInstance_CB getPluginInstance;
    SK_FindPluginParamByName_CB findPluginParamByName;
    SK_FindPluginParamIdxByName_CB findPluginParamIdxByName;

    SK_PopupCtxMenu_CB popupContextMenu;
    
    SK_InitSK_CB initSK;
    SK_DestroySK_CB destroySK;
    SK_OBJCPPSafeTicker_CB OBJCPPSafeTicker;
    SK_OBJCPPSafeInitializer_CB OBJCPPSafeInitializerCB;

    SK_tickSK_TimerMngr_CB tickSK_TimerMngr;
    
    SK_enableDebug_Views_CB enableDebug_Views;

    ~SK_Global(){
        delete threadPool;
        threadPool = nullptr;
        
        appInitializer = nullptr;
        mainWindow = nullptr;
        sb_ipc = nullptr;
        sk = nullptr;
        framework_base = nullptr;
        
        #if defined(SK_BUNDLE_MODE_DEEP) || defined(SK_BUNDLE_MODE_SHALLOW)
            bundle_library = nullptr;
        #endif
        
        machine = nullptr;

        //delete syncTimer;
        //delete timerMngr;
    }
};

END_SK_NAMESPACE

#include "utils/sk_machine.hpp"

#include "sk_ipc/sk_ipc.hpp"

#include "sk_webview/sk_webview.h"
#include "sk_window_mngr/sk_window_mngr.h"

#include "sk_app_initializer/sk_app_initializer.h"



#include "../module_system/cpp/modules/sk/vfs/sk_ms_vfs_file.hpp"

#include "../module_system/cpp/modules/nodejs/sk_ms_nodejs_os.hpp"
#include "../module_system/cpp/modules/sk/sk_ms_application.hpp"
#include "../module_system/cpp/modules/sk/vfs/sk_ms_vfs.hpp"
#include "../module_system/cpp/modules/sk/sk_ms_bdfs.hpp"
#include "../module_system/cpp/modules/nodejs/sk_ms_nodejs_fs.hpp"
#include "../module_system/cpp/modules/sk/sk_ms_web.hpp"
#include "../module_system/cpp/modules/sk/sk_ms_debugMngr.hpp"

#include "../module_system/cpp/modules/sk/nativeActions/sk_nativeAction_root.hpp"
#include "../module_system/cpp/modules/sk/nativeActions/sk_ms_nativeActions.hpp"

//#include "../module_system/cpp/modules/sk/viewMngr/sk_ms_view.hpp"
//#include "../module_system/cpp/modules/sk/viewMngr/sk_ms_viewMngr.hpp"

#include "../module_system/cpp/modules/sk/protonjs/sk_ms_protonjs_native_image.hpp"
#include "../module_system/cpp/modules/sk/protonjs/sk_ms_protonjs_app.hpp"
#include "../module_system/cpp/modules/sk/protonjs/sk_ms_protonjs_window.hpp"
#include "../module_system/cpp/modules/sk/protonjs/sk_ms_protonjs.hpp"

#include "../module_system/sk_module_system.hpp"


#include "sk_webview_initializer/sk_webview_initializer.hpp"


#include "sk_communication/sk_communication.hpp"
