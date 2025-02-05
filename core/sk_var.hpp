#pragma once


//Stringify macro
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)


//Define namespace
#define BEGIN_SK_NAMESPACE namespace SK {
#define END_SK_NAMESPACE }


//Define which OS is being used
#if defined(_WIN32) || defined(_WIN64)
	#define SK_OS "windows"
	#define SK_OS_windows
#elif defined(__APPLE__)
	#include <TargetConditionals.h>
	#if TARGET_OS_IPHONE
	#define SK_OS "ios"
		#define SK_OS_ios
	#else
	#define SK_OS "macos"
		#define SK_OS_macos
	#endif
#elif defined(__linux__)
	#define SK_OS "linux"
	#define SK_OS_linux
#elif defined(__ANDROID__)
	#define SK_OS "android
	#define SK_OS_android
#else
	#define SK_OS "unknown"
	#define SK_OS_unknown
#endif


//Defined app type

#define SK_APP_TYPE "app"
#define SK_APP_TYPE_app

//Define which framework is being used
#define SK_FRAMEWORK "Superkraft" //Default
#define SK_FRAMEWORK_Superkraft //Default

#if __has_include("../../../iPlug2_SK/IPlug/IPlugConstants.h")
	#include "config.h"
    #undef SK_FRAMEWORK_Superkraft
	#define SK_FRAMEWORK "iPlug2"
	#define SK_FRAMEWORK_iPlug2
	#undef SK_FRAMEWORK_Superkraft

	#ifdef VST2_API
		#undef SK_APP_TYPE_app

		#define SK_APP_TYPE "vst"
		#define SK_APP_TYPE_vst
		#define SK_APP_TYPE_vst2


	#elif defined AU_API
		#undef SK_APP_TYPE_app

		#define SK_APP_TYPE "au"
		#define SK_APP_TYPE_au
		#define SK_APP_TYPE_au1


	#elif defined AUv3_API
		#undef SK_APP_TYPE_app

		#define SK_APP_TYPE "au"
		#define SK_APP_TYPE_au
		#define SK_APP_TYPE_au3


	#elif defined AAX_API
		#undef SK_APP_TYPE_app

		#define SK_APP_TYPE "aax"
		#define SK_APP_TYPE_aax


	#elif defined APP_API
		#undef SK_APP_TYPE_app

		#define SK_APP_TYPE "app"
		#define SK_APP_TYPE_app


	#elif defined WAM_API
		#undef SK_APP_TYPE_app

		#define SK_APP_TYPE "wam"
		#define SK_APP_TYPE_wam


	#elif defined WEB_API
		#undef SK_APP_TYPE_app

		#define SK_APP_TYPE "webapi"
		#define SK_APP_TYPE_webapi


	#elif defined VST3_API
		#undef SK_APP_TYPE_app

		#define SK_APP_TYPE "vst"
		#define SK_APP_TYPE_vst
		#define SK_APP_TYPE_vst3


	#elif defined VST3C_API
		#undef SK_APP_TYPE_app

		#define SK_APP_TYPE "vst"
		#define SK_APP_TYPE_vst
		#define SK_APP_TYPE_vst3


	#elif defined VST3P_API
		#undef SK_APP_TYPE_app

		#define SK_APP_TYPE "vst"
		#define SK_APP_TYPE_vst
		#define SK_APP_TYPE_vst3


	#elif defined CLAP_API
		#undef SK_APP_TYPE_app

		#define SK_APP_TYPE "clap"
		#define SK_APP_TYPE_clap
	#else
	#endif
#endif

#if __has_include("JuceHeader.h")
	#include <JuceHeader.h>
	#define SK_FRAMEWORK "JUCE"
	#define SK_FRAMEWORK_JUCE
	#undef SK_FRAMEWORK_Superkraft
#endif


//Define which mode
#if defined DEBUG || defined _DEBUG
	#define SK_MODE_DEBUG
	#define SK_MODE "debug"
#else
	#define SK_MODE_RELEASE
	#define SK_MODE "runtime"
#endif





//Define CPU architecture
#if defined(__x86_64__) || defined(_M_X64)
	#define SK_CPU_ARCH x64
#elif defined(__i386__) || defined(_M_IX86)
	#define SK_CPU_ARCH x86
#elif defined(__arm__) || defined(_M_ARM)
	#define SK_CPU_ARCH arm
#elif defined(__aarch64__) || defined(_M_ARM64)
	#define SK_CPU_ARCH arm64
#elif defined(__ia64__) || defined(_M_IA64)
	#define SK_CPU_ARCH ia64
#elif defined(__mips__) || defined(__mips)
	#if defined(__LP64__) || defined(_LP64)
		#define SK_CPU_ARCH mips64
	#else
		#define SK_CPU_ARCH mips
	#endif
#elif defined(__mipsel__) || defined(__mips_le)
	#if defined(__LP64__) || defined(_LP64)
		#define SK_CPU_ARCH mips64el
	#else
		#define SK_CPU_ARCH mipsel
	#endif
#elif defined(__powerpc__) || defined(__powerpc64__)
	#if defined(__PPC64__)
		#define SK_CPU_ARCH ppc64
	#else
		#define SK_CPU_ARCH ppc
	#endif
#elif defined(__s390__) || defined(__s390x__)
	#if defined(__s390x__)
		#define SK_CPU_ARCH s390x
	#else
		#define SK_CPU_ARCH s390
	#endif
#elif defined(__riscv)
	#if __riscv_xlen == 64
		#define SK_CPU_ARCH riscv64
	#else
		#define SK_CPU_ARCH riscv32
	#endif
#else
	#define SK_CPU_ARCH unknown
#endif



#define SK_THREAD_POOL_COUNT 8
