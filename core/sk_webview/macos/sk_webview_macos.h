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

        @interface SK_WebView_MacOS : WKWebView
            @property (nonatomic, strong) NSString *contextualMenuActionIdentifier;
            @property (nonatomic, assign) void* sk_webview_parent;
        @end

        NS_ASSUME_NONNULL_END
    #endif
#endif




BEGIN_SK_NAMESPACE

class SK_WebView_SharedBuffer {
public:
    size_t uuid;
    bool busy = false;
    nlohmann::json metadata;
    SK_Communication_Response_Web* buffer = nullptr;
    
    SK_WebView_SharedBuffer(size_t idx){
        buffer = new SK_Communication_Response_Web(SK_Base_URL + "/__sk_sharedBuffer?id=" + std::to_string(idx));
        buffer->headers["Content-Type"] = "application/octet-stream";
        buffer->setAsOK();
    }
    
    ~SK_WebView_SharedBuffer(){
        delete buffer;
    }
};

class SK_WebView {
public:
    using SK_WebView_EvaluationComplete_Callback = std::function<void(const SK_String& result)>;
    
    SK_Global* _Nullable skg;
    
    void* parentWnd;
    
    
    bool debugEnabled = false;
    int debugKeyPressCount = 0;
    SK_Timer* debugActivatorTimer;
    
    #if defined(SK_OS_macos)
        #ifdef __OBJC__
            NSWindow* _Nullable parentWndHandle;
            NSView* _Nullable parentContentView;
            __strong SK_WebView_MacOS* _Nullable webview;
            __strong SK_Webview_MacOS_Delegate* _Nullable webviewDelegate;
    
            __strong SK_WebView_URLSchemeHandler* _Nullable messageHandler;
            __strong SK_WebView_URLSchemeHandler* _Nullable urlHandler;
        #endif
    #endif
    
    SK_String tag = "some_id";
    SK_String currentURL = "";
    
    
    SK_WebView_Simple_Callback notifyReadyToShow;
    
    bool isReady = false;
    bool isShuttingDown = false;
    bool sharedBuffersCanBeShared = false;
    
    SK_Communication_Response_Web* _Nullable readyStateWebResponse = nullptr;
    size_t sharedBuffersIdx = 0;
    std::unordered_map<size_t, SK_WebView_SharedBuffer*> sharedBuffersQueue;
    SK_Timer* sharedBuffersTimer = nullptr;
    SK_Timer::CallbackId sharedBuffersTimerCallbackId = 0;
    SK_Timer::CallbackId debugActivatorTimerCallbackId = 0;
    
    ~SK_WebView();
    void shutdown();
    
    void create(bool offsetWhenDebugging);
    void update();
    void navigate(const SK_String& url);
    void showDevTools();
    void evaluateScript_mainThread(void* _Nonnull _webview, const SK_String& src, SK_WebView_EvaluationComplete_Callback cb);
    void evaluateScript(const SK_String& src, SK_WebView_EvaluationComplete_Callback cb);
    void sendMsgAsJSON_mainThread(void* _Nonnull _webview, const SK_String& src, SK_WebView_EvaluationComplete_Callback cb);
    void sendMsgAsJSON(const SK_String& src, SK_WebView_EvaluationComplete_Callback cb);
    
    void addBufferToQueue(size_t size, void* data, const nlohmann::json& metadata);
    SK_WebView_SharedBuffer* getBufferAndRemove(size_t uuid);
    void sendSharedBufferOnMainThread(size_t size, void* _Nonnull data, const nlohmann::json& metadata);
    void sendSharedBuffer(size_t size, void* _Nonnull data, const nlohmann::json& metadata);
    void tryStartingSharedBuffersTimer();
    
    void configDebugging();
    void enableDebug(const bool& enable);
    void tryActivateDebug();
};

END_SK_NAMESPACE
