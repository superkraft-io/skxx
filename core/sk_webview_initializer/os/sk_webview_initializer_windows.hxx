#pragma once

#include "../../sk_common.hxx"

BEGIN_SK_NAMESPACE

class SK_WebView_Initializer {
public:
    SK_Module_System* modsys;

    wil::com_ptr<ICoreWebView2> webview;

    void init(void* _webview, bool isHardBackend){
        webview = static_cast<ICoreWebView2*>(_webview);
        inject_core();

        webview = nullptr;
    }

    void inject_core(){
        SK_String modulesRoot = "https://sk.sb.gc";

        injectData("window.sk_api = {}");

        SK_String payload = generateFromFiles(std::vector<SK_String>{
            SK_Path_Utils::paths["global_js_core"] + "/sk_ipc.js",
            SK_Path_Utils::paths["module_system"] + "/sk_module.js",
            SK_Path_Utils::paths["module_system"] + "/sk_module_root.js",
            SK_Path_Utils::paths["global_js_core"] + "/sk_global_js_core.js"
        })
        .replace("'<sk_static_info>'", getStaticInfo())
        .replace("'<sk_native_actions>'", modsys->nativeActions.listActions());

        injectData(payload);
    }

    SK_String generateFromFiles(std::vector<SK_String>& paths){
        SK_String data;

        for (int i = 0; i < paths.size(); i++) {
            SK_File file;

            #ifdef SK_MODE_DEBUG
                file.loadFromDisk(paths[i]);
            #else
            #endif

            data += "\n\r" + file;
        }


        return data;
    }

    void injectData(const SK_String& data){
        webview->AddScriptToExecuteOnDocumentCreated(
            data,
            Callback<ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler>(
                [this](HRESULT error, PCWSTR id) -> HRESULT {
                    return S_OK;
                }
            ).Get()
       );
    }

    nlohmann::json getAppInfo() {
        SK_String appName = sk_config["product_info"]["name"];
        SK_String appVersion = sk_config["product_info"]["version"];

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
            {"machine", SK_Machine::getStaticInfo()}
        };


        return res.dump();
    }
};


END_SK_NAMESPACE
