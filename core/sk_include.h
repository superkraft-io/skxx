#pragma once


#include <unordered_map>
#include <optional>
#include <filesystem>
#include <regex>
#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <iomanip>
#include <map>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <iterator>
#include <chrono>
#include <ctime>
#include <fstream>
#include <bitset>
#include <memory>
#include <variant>

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cmath>


#include "wdlstring.h"


#include "../libs/general/debugbreak.h"
#include "sk_var.hpp"


#if defined(SK_OS_windows)
    #include <vld.h>
#endif

#include "json.hpp"


#if defined(SK_OS_windows)
	#include <windows.h>
	#include <windowsx.h>

	#include <strsafe.h>
	
	#include <wrl.h>
	#include <wil/com.h>
	#include "WebView2.h"
	#include "WebView2EnvironmentOptions.h"

	#include <VersionHelpers.h>
	#include <comdef.h>
	#include <Wbemidl.h>
	#include <lmcons.h>
	#include <intrin.h>

	#include <shlobj.h>

	#include <pdh.h>
	#include <pdhmsg.h>
	#include <iostream>

	#pragma comment(lib, "pdh.lib")
	#pragma comment(lib, "wbemuuid.lib")

	#pragma comment(lib, "ole32.lib")

	#include <dwmapi.h>
	#pragma comment(lib, "dwmapi.lib")

	typedef NTSTATUS(WINAPI* RtlGetVersionFunc)(RTL_OSVERSIONINFOEXW*);

	#pragma comment(lib, "Ws2_32.lib")


   #include "utils/sk_string/sk_string_windows.hpp"
#else
	#include <sys/utsname.h>
	#include <unistd.h>
	#include <sys/sysctl.h>
	#include <sys/types.h>
	#include <mach/mach.h>
	#include <pwd.h>
	#include <cstdlib>
	#include <sys/stat.h>

	#if defined (SK_OS_macos) || defined (SK_OS_ios)
		#include <TargetConditionals.h>
        
     
        #include "utils/sk_string/sk_string_apple.hpp"
        
        
    
	#elif defined (SK_OS_linux) || defined (SK_OS_android)
		// Linux specific includes
	#endif
#endif



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




 
static nlohmann::json sk_config;

BEGIN_SK_NAMESPACE

class SK_Common {
public:
	static inline SK_String runningAs = SK_String("unknown");

	static inline SK_Window_onWindowFocusChanged_Callback onWindowFocusChanged;

	static inline SK_WindowMngr_onFindWindowByString onFindWindowByClassName;
	static inline SK_WindowMngr_onFindWindowByString onFindWindowByTag;
	

	static inline SK_WebView_OnReady onWebViewReady;

	static inline SK_Communication_onRequest onCommunicationRequest;


    #if defined(SK_OS_windows)
        static inline SK_WindowMngr_updateWebViewHWNDListForView updateWebViewHWNDListForView;
        static inline SK_onMainWindowHWNDAcquired onMainWindowHWNDAcquired;
        static inline SK_WindowMngr_getWebview2HWNDForWindow getWebview2HWNDForWindow;
        static inline HWND mainWindowHWND;
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
#include "sk_window_mngr/root/sk_window_root.hpp"



#if defined(SK_OS_windows)
    #include "sk_window_mngr/windows/sk_window_windows.hpp"
#elif defined(SK_OS_macos) || defined(SK_OS_ios)
    #include "sk_window_mngr/macos/sk_window_macos.hpp"
#endif
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

