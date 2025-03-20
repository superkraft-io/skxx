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

//#include "../libs/general/yyjson/yyjson.h"
//#include "../libs/general/glaze/glaze.hpp"
//#include "sk_json/sk_json_glaze.hpp"

#include "sk_profiler/sk_profiler.hpp"

#include "utils/sk_point.hpp"


#include "sk_web/sk_web_utils.hpp"
#include "utils/sk_file.hpp"

#include "sk_callbacks.hpp"

#include "sk_threads/sk_thread_pool.hpp"


#include "../libs/general/curl/curl.h"
#include "sk_web/sk_curl.hpp"


#include "sk_communication/sk_communication_response.hpp"
#include "sk_communication/sk_communication_packet.hpp"


#if defined(SK_OS_macos)
    #ifdef __OBJC__
        #import <Foundation/Foundation.h>
        #import <AppKit/AppKit.h>
        #import <WebKit/WebKit.h>
    #endif
#endif

BEGIN_SK_NAMESPACE

//class SK_Machine;

class SK_Global {
public:
    static SK_Global& GetInstance() {
        static SK_Global instance;
        return instance;
    }

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
    }

    SK_Path_Utils pathUtils;
    void* machine;

    nlohmann::json sk_config;
    
    long long ipc_msg_id = 0;

	SK_String runningAs = SK_String("unknown");

    void* project;

    void* sk = nullptr;


	SK_Window_onWindowFocusChanged_Callback onWindowFocusChanged;

	SK_WindowMngr_onFindWindowByString onFindWindowByClassName;
	SK_WindowMngr_onFindWindowByString onFindWindowByTag;
	
    SK_WebView_SendMsgToFrontend_CB sendMsgToWebview;
	SK_WebView_OnReady onWebViewReady;

	SK_Communication_onRequest onCommunicationRequest;

    
    
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

	SK_ThreadPool_ProcessMainThreadTasks threadPool_processMainThreadTasks = []() {
        SK_Global::GetInstance().threadPool->processMainThreadTasks();
	};

	SK_showSoftBackendDevTools showSoftBackendDevTools;

	SK_resizeAllMainWindowViews resizeAllMainWindowViews;

	SK_getMainWindowSize getMainWindowSize;
	SK_setMainWindowSize setMainWindowSize;

    SK_onPreConfigWnd onPreConfigWnd = NULL;
    SK_onPostConfigWnd onPostConfigWnd = NULL;
    SK_wndCreated onWndCreated = NULL;

    SK_WebView_onGetUserDataPath onGetWebViewUserDataPath;
private:
    SK_Global() {}
    ~SK_Global() {}
    SK_Global(const SK_Global&) = delete;
    SK_Global& operator=(const SK_Global&) = delete;
};

END_SK_NAMESPACE

#include "utils/sk_machine.hpp"

#include "sk_ipc/sk_ipc_v2.hpp"

#include "sk_webview/sk_webview.h"
#include "sk_window_mngr/sk_window_mngr.hpp"

#include "sk_app/sk_app_initializer.h"

#include "../../sk_project_binarydata.hpp"


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
