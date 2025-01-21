#pragma once

#if defined(SK_OS_windows)
	#include "os/sk_webview_initializer_windows.hxx"
#elif defined(SK_OS_macos)
	#include "os/sk_webview_initializer_macos.hxx"
#elif defined(SK_OS_ios)
	#include "os/sk_webview_initializer_ios.hxx"
#elif defined(SK_OS_linux)
	#include "os/sk_webview_initializer_linux.hxx"
#elif defined(SK_OS_android)
	#include "os/sk_webview_initializer_android.hxx"
#endif