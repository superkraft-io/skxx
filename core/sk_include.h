#pragma once

#include "sk_include_core.h"

#include "utils/sk_string/sk_string.h"
#include "utils/sk_number.hpp"
#include "utils/sk_path_utils.hpp"
#include "utils/sk_array.hpp"
#include "utils/sk_datetime.hpp"
#include "utils/sk_str_utils.hpp"
#include "utils/sk_color.hpp"
#include "utils/sk_machine.hpp"

#include "sk_json/sk_json_callback.hpp"

#include "../libs/general/yyjson/yyjson.h"

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
#include "sk_ipc/sk_ipc_v2.hpp"




BEGIN_SK_NAMESPACE

class SK_Global {
public:
    static inline nlohmann::json sk_config;
    
	static inline SK_String runningAs = SK_String("unknown");

	static inline SK_Window_onWindowFocusChanged_Callback onWindowFocusChanged;

	static inline SK_WindowMngr_onFindWindowByString onFindWindowByClassName;
	static inline SK_WindowMngr_onFindWindowByString onFindWindowByTag;
	

	static inline SK_WebView_OnReady onWebViewReady;

	static inline SK_Communication_onRequest onCommunicationRequest;

    
    
    static inline SK_onMainWindowHWNDAcquired onMainWindowHWNDAcquired;
    static inline HWND mainWindowHWND;

    #if defined(SK_OS_windows)
        static inline SK_WindowMngr_updateWebViewHWNDListForView updateWebViewHWNDListForView;
        static inline SK_WindowMngr_getWebview2HWNDForWindow getWebview2HWNDForWindow;
    #endif

	static inline SK_IPC_v2* sb_ipc;

	static inline SK_Thread_Pool* threadPool = new SK_Thread_Pool(8);

	static inline SK_ThreadPool_ProcessMainThreadTasks threadPool_processMainThreadTasks = []() {
		threadPool->processMainThreadTasks();
	};

	static inline SK_showSoftBackendDevTools showSoftBackendDevTools;

	static inline SK_resizeAllMianWindowView resizeAllMianWindowView;

	static inline SK_getMainWindowSize getMainWindowSize;
	static inline SK_setMainWindowSize setMainWindowSize;
};

END_SK_NAMESPACE



#include "sk_webview/sk_webview.h"
#include "sk_window_mngr/sk_window_mngr.hpp"



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

#include "../module_system/cpp/modules/sk/viewMngr/sk_ms_view.hpp"

#include "../module_system/cpp/modules/sk/viewMngr/sk_ms_viewMngr.hpp"

#include "../module_system/cpp/modules/sk/protonjs/sk_ms_protonjs_native_image.hpp"
#include "../module_system/cpp/modules/sk/protonjs/sk_ms_protonjs.hpp"

#include "../module_system/sk_module_system.hpp"


#include "sk_webview_initializer/sk_webview_initializer.hpp"


#include "sk_communication/sk_communication.hpp"
