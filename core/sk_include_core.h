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

#include "../../sk_target_build_defs.h"

#include "../../sk_project_includes.hpp"

#include "json.hpp"


#if defined(SK_OS_windows)
	#include <vld.h>
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


#else
	#include <sys/utsname.h>
	#include <unistd.h>
	#include <sys/sysctl.h>
	#include <sys/types.h>
	#include <mach/mach.h>
	#include <pwd.h>
	#include <cstdlib>
	#include <sys/stat.h>

	#if defined (SK_OS_apple)
		#ifdef __OBJC__
			#import <Foundation/Foundation.h>
			#import <AppKit/AppKit.h>
			#import <WebKit/WebKit.h>
		#endif
	#elif defined (SK_OS_linux) || defined (SK_OS_android)
		// Linux specific includes
	#endif
#endif
