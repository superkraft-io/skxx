#pragma once

#include "../../sk_common.hpp"


#if defined(SK_OS_macos)
    #ifdef __OBJC__
        #import <AppKit/AppKit.h>
        #import <WebKit/WebKit.h>
    #endif
#endif




BEGIN_SK_NAMESPACE

class SK_WebView {
public:
    using SK_WebView_EvaluationComplete_Callback = std::function<void(const SK_String& result)>;

    
    #if defined(SK_OS_macos)
        #ifdef __OBJC__
            NSWindow* parentWnd;
            WKWebView* webview;
        #endif
    #endif
    
    
    std::string currentURL = "";
    
    void create();
    void update();
    void navigate(std::string url);
    void evaluateScript_mainThread(void* _webview, const SK_String& src, SK_WebView_EvaluationComplete_Callback cb);
    void evaluateScript(const SK_String& src, SK_WebView_EvaluationComplete_Callback cb);
    void showDevTools();
};

END_SK_NAMESPACE
