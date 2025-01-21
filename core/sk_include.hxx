#pragma once

#include <vld.h>

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



#include "wdlstring.h"

#include "sk_var.hxx"

#include "json.hpp"
//#include "../libs/general/yyjson/src/yyjson.h"
//#include "sk_json/sk_json.hxx"

#if defined(SK_OS_windows)

#include <windows.h>
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
		#include <sys/stat.h>
		#include <unistd.h>
	#elif defined (SK_OS_linux) || defined (SK_OS_android)
		// Linux specific includes
	#endif
#endif



#include "../libs/general/choc/text/choc_StringUtilities.h"


#include "utils/sk_string.hxx"
#include "utils/sk_path_utils.hxx"
#include "utils/sk_array.hxx"
#include "utils/sk_datetime.hxx"
#include "utils/sk_str_utils.hxx"
#include "utils/sk_machine.hxx"


#include "sk_profiler/sk_profiler.hxx"


#include "sk_web/sk_web_utils.hxx"
#include "utils/sk_file.hxx"

#include "sk_callbacks.hxx"

#include "sk_threads/sk_thread_pool.hxx"

#include "../libs/general/curl/curl.h"
#include "sk_web/sk_curl.hxx"


#include "sk_communication/sk_communication_response.hxx"
#include "sk_communication/sk_communication_packet.hxx"
#include "sk_ipc/sk_ipc_v2.hxx"




static nlohmann::json sk_config;

BEGIN_SK_NAMESPACE

class SK_Common {
public:
	static inline SK_String runningAs = SK_String("unknown");

	static inline SK_Window_onWindowFocusChanged_Callback onWindowFocusChanged;

	static inline SK_WindowMngr_onFindByWindowClassName onFindByWindowClassName;
	static inline SK_WindowMngr_updateWebViewHWNDListForView updateWebViewHWNDListForView;
	static inline SK_WindowMngr_getWebview2HWNDForWindow getWebview2HWNDForWindow;

	static inline SK_onMainWindowHWNDAcquired onMainWindowHWNDAcquired;

	static inline SK_WebView_OnReady onWebViewReady;

	static inline SK_Communication_onRequest onCommunicationRequest;
#if defined(SK_OS_windows)
	static inline HWND mainWindowHWND;
#endif

	static inline SK_IPC_v2* sb_ipc;

	static inline SK_Thread_Pool* threadPool = new SK_Thread_Pool(8);

	static inline SK_ThreadPool_ProcessMainThreadTasks threadPool_processMainThreadTasks = []() {
		threadPool->processMainThreadTasks();
	};
};

END_SK_NAMESPACE



#include "sk_webview/sk_webview.h"
#if defined(SK_FRAMEWORK_iPlug2)
	#include "sk_window_mngr/root/sk_window_root.hxx"
#else
	#include "../root/sk_window_root.h"
#endif

#include "sk_window_mngr/windows/sk_window_windows.hxx"
#include "sk_window_mngr/sk_window_mngr.hxx"



#include "../../../sk_project_binarydata.hxx"


#include "../module_system/cpp/modules/sk/vfs/sk_ms_vfs_file.hxx"

#include "../module_system/cpp/modules/nodejs/os/sk_ms_nodejs_os.hxx"
#include "../module_system/cpp/modules/sk/application/sk_ms_application.hxx"
#include "../module_system/cpp/modules/sk/vfs/sk_ms_vfs.hxx"
#include "../module_system/cpp/modules/sk/bdfs/sk_ms_bdfs.hxx"
#include "../module_system/cpp/modules/nodejs/fs/sk_ms_nodejs_fs.hxx"
#include "../module_system/cpp/modules/sk/web/sk_ms_web.hxx"

#include "../module_system/cpp/modules/sk/nativeActions/sk_nativeAction_root.hxx"
#include "../module_system/cpp/modules/sk/nativeActions/sk_ms_nativeActions.hxx"

#include "../module_system/cpp/modules/sk/viewMngr/sk_ms_view/sk_ms_view.hxx"

#include "../module_system/cpp/modules/sk/viewMngr/sk_ms_viewMngr.hxx"

#include "../module_system/sk_module_system.hxx"

#include "sk_webview_initializer/os/sk_webview_initializer_windows.hxx"

#include "sk_communication/sk_communication.hxx"