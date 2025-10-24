#pragma once

#include "IPlugParameter.h"

//#define SK_ROUTE_FS_TO_BDFS 1
//#define SK_BUNDLE_MODE_SHALLOW 1
//static std::string SK_BUNDLER_SHALLOW_DATA_PATH("/Users/splitterai/Desktop/repos/superkraft-io/skxx_iplug2_demo/sk_soft_backend_bundle/shallow/groups/data");

#if defined(SK_OS_apple)
    #include "IPlugSWELL.h"
#endif

#if __has_include("IPlugConstants.h")
    #include "config.h"
    #undef SK_FRAMEWORK_Superkraft
    #define SK_FRAMEWORK "iPlug2"
    #define SK_FRAMEWORK_iPlug2
    #undef SK_FRAMEWORK_Superkraft

    #if defined(VST2_API)
        #undef SK_APP_TYPE_app

        #define SK_APP_TYPE "plugin"
        #define SK_APP_TYPE_plugin
        #define SK_APP_TYPE_vst
        #define SK_APP_TYPE_vst2


    #elif defined(AU_API)
        #undef SK_APP_TYPE_app

        #define SK_APP_TYPE "plugin"
        #define SK_APP_TYPE_plugin
        #define SK_APP_TYPE_au
        #define SK_APP_TYPE_au2


    #elif defined(AUv3_API)
        #undef SK_APP_TYPE_app

        #define SK_APP_TYPE "plugin"
        #define SK_APP_TYPE_plugin
        #define SK_APP_TYPE_au
        #define SK_APP_TYPE_au3


    #elif defined(AAX_API)
        #undef SK_APP_TYPE_app

        #define SK_APP_TYPE "plugin"
        #define SK_APP_TYPE_plugin
        #define SK_APP_TYPE_aax


    #elif defined(APP_API)
        #undef SK_APP_TYPE_app

        #define SK_APP_TYPE "app"
        #define SK_APP_TYPE_app


    #elif defined WAM_API
        #undef SK_APP_TYPE_app

        #define SK_APP_TYPE "wam"
        #define SK_APP_TYPE_wam


    #elif defined(WEB_API)
        #undef SK_APP_TYPE_app

        #define SK_APP_TYPE "webapi"
        #define SK_APP_TYPE_webapi


    #elif defined(VST3_API)
        #undef SK_APP_TYPE_app

        #define SK_APP_TYPE "plugin"
        #define SK_APP_TYPE_plugin
        #define SK_APP_TYPE_vst
        #define SK_APP_TYPE_vst3


    #elif defined(VST3C_API)
        #undef SK_APP_TYPE_app

        #define SK_APP_TYPE "plugin"
        #define SK_APP_TYPE_plugin
        #define SK_APP_TYPE_vst
        #define SK_APP_TYPE_vst3


    #elif defined(VST3P_API)
        #undef SK_APP_TYPE_app

        #define SK_APP_TYPE "plugin"
        #define SK_APP_TYPE_plugin
        #define SK_APP_TYPE_vst
        #define SK_APP_TYPE_vst3


    #elif defined(CLAP_API)
        #undef SK_APP_TYPE_app

        #define SK_APP_TYPE "plugin"
        #define SK_APP_TYPE_plugin
        #define SK_APP_TYPE_clap
    #else
    #endif
#endif


using SK_GetPluginInstance_CB = std::function<void*()>;
using SK_FindPluginParamByName_CB = std::function<iplug::IParam*(const std::string& paramName)>;
using SK_FindPluginParamIdxByName_CB = std::function<int(const std::string& paramName)>;

using SK_PopupCtxMenu_CB = std::function<void(int paramIdx, int x, int y)>;
