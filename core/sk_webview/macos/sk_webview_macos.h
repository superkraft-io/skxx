#pragma once

#include "../../sk_common.hpp"

#if defined(SK_OS_macos)
    #ifdef __OBJC__
        #import <AppKit/AppKit.h>
        #import <WebKit/WebKit.h>

        NS_ASSUME_NONNULL_BEGIN

        @interface SK_Webview_MacOS_Delegate : NSObject <WKUIDelegate>
            @property (nonatomic, weak) NSWindow* windowHandle; // Use 'weak' to avoid strong reference cycles
        @end

        @interface SK_WebView_URLSchemeHandler : NSObject <WKURLSchemeHandler, WKScriptMessageHandler>
            @property (nonatomic, assign) SK::SK_Global* skg;
            @property (nonatomic, assign) void* webView;
            @property (nonatomic, assign) SK::SK_String tag;
        @end

        NS_ASSUME_NONNULL_END
    #endif
#endif




BEGIN_SK_NAMESPACE

class SK_WebView {
public:
    using SK_WebView_EvaluationComplete_Callback = std::function<void(const SK_String& result)>;
    
    SK_Global* _Nullable skg;
    
    #if defined(SK_OS_macos)
        #ifdef __OBJC__
            NSWindow* _Nullable parentWndHandle;
            NSView* _Nullable parentContentView;
            WKWebView* _Nullable webview;
            __strong SK_Webview_MacOS_Delegate* _Nullable webviewDelegate;
    
            SK_WebView_URLSchemeHandler* messageHandler;
            SK_WebView_URLSchemeHandler* urlHandler;
        #endif
    #endif
    
    SK_String tag = "some_id";
    SK_String currentURL = "";
    
    
    SK_WebView_Simple_Callback notifyReadyToShow;
    
    ~SK_WebView();
    
    void create(bool offsetWhenDebugging);
    void update();
    void navigate(const SK_String& url);
    void evaluateScript_mainThread(void* _Nonnull _webview, const SK_String& src, SK_WebView_EvaluationComplete_Callback cb);
    void evaluateScript(const SK_String& src, SK_WebView_EvaluationComplete_Callback cb);
    void showDevTools();
};

END_SK_NAMESPACE
