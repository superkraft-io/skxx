#pragma once

#include "../../sk_common.hpp"

#import <WebKit/WebKit.h>

@class SK_WebView_URLSchemeHandler;

BEGIN_SK_NAMESPACE

class SK_WebView {
public:
    using SK_WebView_EvaluationComplete_Callback = std::function<void(const SK_String& result)>;
    
    NSWindow *parentWnd;
    WKWebView *webview;
    
    std::string currentURL = "";
    
    void create();
    void handleScriptMessage(WKScriptMessage* message);
    void update();
    void navigate(std::string url);
    void evaluateScript_mainThread(WKWebView* _webview, const SK_String& src, SK_WebView_EvaluationComplete_Callback cb);
    void evaluateScript(const SK_String& src, SK_WebView_EvaluationComplete_Callback cb);
    void showDevTools();
};

END_SK_NAMESPACE
