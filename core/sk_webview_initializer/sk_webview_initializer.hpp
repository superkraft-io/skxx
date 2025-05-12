#pragma once

#include "../sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_WebView_Initializer {
public:
    SK_Global* skg;

    SK_Module_System* modsys;

    SK_String pluginParameters = "";

   

    
    ~SK_WebView_Initializer(){
        modsys = nullptr;
        skg = nullptr;
    }
    
    
    
    #if defined(SK_OS_windows)
        wil::com_ptr<ICoreWebView2> castWebView(void* webview) {
            if (!webview) return nullptr;
            try {
                auto p = static_cast<ICoreWebView2*>(webview);
                if (p) p->AddRef(); // Explicit refcount management
                return wil::com_ptr<ICoreWebView2>(p, wil::AddRefPolicy::No);
            } catch (...) {
                return nullptr;
            }
        }
    #elif defined(SK_OS_apple) && defined(__OBJC__)
        WKWebView* castWebView(void* webview) {
            if (!webview) return nil;
            return (__bridge WKWebView*)webview;
        }
    #endif
    
    
    
    void init(void* webview, bool isHardBackend){
        inject_core(webview);
    }

    void inject_core(void* webview){
        #if defined(SK_OS_windows)
            injectData("window.__SK_IPC_Send  = data => { window.chrome.webview.postMessage(data) }");
        #endif
        
        injectData(webview, "window.sk_api = {}");

        SK_Path_Utils* pathUtils = &skg->pathUtils;
        SK_String payload = generateFromFiles(std::vector<SK_String>{
            pathUtils->paths["global_js_core"] + "/sk_ipc.js",
            pathUtils->paths["module_system"] + "/sk_module.js",
            pathUtils->paths["module_system"] + "/sk_module_root.js",
            
            pathUtils->paths["global_js_core"] + "/sk_dawPluginMngr.js",

            pathUtils->paths["global_js_core"] + "/sk_global_js_core.js",
            
            pathUtils->paths["global_js_core"] + "/sk_debug_mode.js"
        })
        #if defined(SK_APP_TYPE_plugin)
            .replace("'<sk_plugin_parameters>'", pluginParameters)
           
                #if defined(SK_OS_windows)
                    .replace("/* SK_OS_windows - START", "//SK_OS_windows - START")
                    .replace("SK_OS_windows - END */", "//SK_OS_windows - END")
                #elif defined(SK_OS_apple)
                    //do nothing
                #endif
          
        #endif
        .replace("<sk_base_url>", SK_Base_URL)
        .replace("'<sk_static_info>'", getStaticInfo())
        .replace("'<sk_native_actions>'", modsys->nativeActions->listActions());

        injectData(webview, payload);
    }

    SK_String generateFromFiles(const std::vector<SK_String>& paths){
        SK_String data;

        for (int i = 0; i < paths.size(); i++) {
            SK_File file;

            #ifdef SK_MODE_DEBUG
                file.loadFromDisk(paths[i]);
            #else
                //..
            #endif

            data += "\n\r" + file;
        }


        return data;
    }

    void injectData(void* webview, const SK_String& data){
        #if defined(SK_OS_windows)
            castWebview(webview)->AddScriptToExecuteOnDocumentCreated(
                data.toWString().c_str(),
                Callback<ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler>(
                    [this](HRESULT error, PCWSTR id) -> HRESULT {
                        return S_OK;
                    }
                ).Get()
           );
        #elif defined(SK_OS_apple)
            #ifdef __OBJC__
                WKUserScript *userScript = [[WKUserScript alloc] initWithSource:data
                                                                  injectionTime:WKUserScriptInjectionTimeAtDocumentStart
                                                               forMainFrameOnly:NO];

                [castWebView(webview).configuration.userContentController addUserScript:userScript];
            #endif
        #endif
        
    }
    
    nlohmann::json getAppInfo() {
        SK_String appName = skg->sk_config["product_info"]["name"];
        SK_String appVersion = skg->sk_config["product_info"]["version"];

        SK_String argv0 = "";
        //if (SK_Superkraft_App::app_argv.length() > 0) argv0 = SK_Superkraft_App::app_argv[0]; //ignoring this for now

        nlohmann::json applicationInfo{
            {"argv"   , "<argv>"},
            {"argv0"  , argv0},
            {"mode"   , SK_MODE},
            {"name"   , appName},
            {"version", appVersion}
        };

        SK_String output = applicationInfo.dump();

        SK_String args = "";//"[\"" + SK_String(sk_app_argv) + "\"]";
        if (args == "[\"\"]") {
            args = "[]";
        }
        output = output.replace("\"<argv>\"", args);


        return applicationInfo;
    }

    SK_String getStaticInfo() {
        nlohmann::json res {
            {"application", getAppInfo()},
            {"machine", static_cast<SK_Machine*>(skg->machine)->getStaticInfo()}
        };


        return res.dump();
    }
};


END_SK_NAMESPACE
