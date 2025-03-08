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


        NS_ASSUME_NONNULL_END
    #endif
#endif




BEGIN_SK_NAMESPACE

class SK_WebView {
public:
    using SK_WebView_EvaluationComplete_Callback = std::function<void(const SK_String& result)>;
    
    #if defined(SK_OS_macos)
        #ifdef __OBJC__
            NSWindow* parentHandle;
            WKWebView* webview;
            __strong SK_Webview_MacOS_Delegate* webviewDelegate;
        #endif
    #endif
    
    SK_String tag = "some_id";
    SK_String currentURL = "";
    
    void create();
    void update();
    void navigate(const SK_String& url);
    void evaluateScript_mainThread(void* _webview, const SK_String& src, SK_WebView_EvaluationComplete_Callback cb);
    void evaluateScript(const SK_String& src, SK_WebView_EvaluationComplete_Callback cb);
    void showDevTools();
};

END_SK_NAMESPACE
