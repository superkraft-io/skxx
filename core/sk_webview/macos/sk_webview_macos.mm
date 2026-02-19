#pragma once

#include "../../sk_common.hpp"
#include "../../superkraft.hpp"

#import <Foundation/Foundation.h>
#import <WebKit/WebKit.h>
#import <AppKit/AppKit.h>

#define WKJSE(key) error.userInfo[@#key]


using namespace SK;

// Keep retired WKWebViews alive permanently.  WebKit's internal
// NavigationState holds a __weak ref to the WKWebView and fires a
// CFRunLoop timer (progress tracker) that calls objc_loadWeakRetained.
// If the WKWebView is deallocated at ANY point while that timer is
// still scheduled, the weak-ref load crashes.  We cannot reliably
// cancel or predict the timer's lifetime (_close causes its own
// crashes, about:blank spawns new timers, and dispatch_after release
// races the timer arbitrarily).  The only safe solution is to keep
// the WKWebView alive forever so the weak ref remains valid until the
// timer naturally drains after stopLoading completes.
static void SK_QuarantineWKWebView(id webViewToRelease)
{
    if (!webViewToRelease) return;

    static NSMutableArray* sReleaseBin = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sReleaseBin = [[NSMutableArray alloc] init];
    });

    [sReleaseBin addObject:webViewToRelease];
    // Intentionally never removed — the WKWebView must stay alive
    // so WebKit's internal __weak references remain valid.
}

@class WKContextMenuElementInfo;

@implementation SK_WebView_URLSchemeHandler
- (void)webView:(WKWebView *)webView startURLSchemeTask:(id <WKURLSchemeTask>)urlSchemeTask {
    SK_String url = urlSchemeTask.request.URL.absoluteString;
    SK_String path = urlSchemeTask.request.URL.path;
    
    
    NSURLComponents *urlComponents = [NSURLComponents componentsWithURL:urlSchemeTask.request.URL resolvingAgainstBaseURL:NO];
    NSMutableDictionary<NSString *, NSString *> *dictionary = [NSMutableDictionary dictionary];
    for (NSURLQueryItem *item in urlComponents.queryItems) {
        dictionary[item.name] = item.value;
    }
    
    
    
    if (url.indexOf("__sk_sharedBuffer") > -1){
        SK_WebView* webview = static_cast<SK_WebView*>(self.webView);
        
        if (!webview) return;
        
        if ([dictionary[@"setReadyState"] isEqualToString:@"true"]){
            webview->sharedBuffersCanBeShared = true;
            SK_Communicaton_Response_Apple res = webview->readyStateWebResponse->getWebResponse();
            [urlSchemeTask didReceiveResponse:res.response];
            [urlSchemeTask didReceiveData:res.data];
            [urlSchemeTask didFinish];
            return;
        }
        
        
        NSString *stringValue = dictionary[@"uuid"];
        NSInteger intValue = [stringValue integerValue];
        size_t uuid = (size_t)intValue;
        SK_WebView_SharedBuffer* sharedBuffer = webview->getBufferAndRemove(uuid);
        
        if (!sharedBuffer) {
            [urlSchemeTask didFailWithError:[NSError errorWithDomain:NSURLErrorDomain code:NSURLErrorResourceUnavailable userInfo:nil]];
            return;
        }
        
        SK_Communicaton_Response_Apple res = sharedBuffer->buffer->getWebResponse();
        [urlSchemeTask didReceiveResponse:res.response];
        [urlSchemeTask didReceiveData:res.data];
        [urlSchemeTask didFinish];
        
        delete sharedBuffer;
        
        return;
    }
    
    SK_Communication_Config config{self.tag, SK_Communication_Packet_Type::sk_comm_pt_web, (__bridge void *)urlSchemeTask.request};
    if (!self.skg) return;
    
    id<WKURLSchemeTask> capturedTask = urlSchemeTask;
    SK::SK_Global* capturedSkg = self.skg;
    SK::SK_String capturedSender = config.sender;
    self.skg->onCommunicationRequest(config, NULL, [capturedTask, capturedSkg, capturedSender](SK_Communication_Packet* packet) -> void* {
        if (packet == nullptr){
            return (static_cast<Superkraft*>(capturedSkg->sk))->comm->packetFromWebRequest(capturedTask.request, capturedSender);
        }
        
        SK_Communication_Response_Web* responseObj = static_cast<SK_Communication_Response_Web*>(packet->response());
        SK_Communicaton_Response_Apple res = responseObj->getWebResponse();
        [capturedTask didReceiveResponse:res.response];
        [capturedTask didReceiveData:res.data];
        [capturedTask didFinish];
        
        return packet;
    });

    return;
}

- (void)webView:(nonnull WKWebView *)webView stopURLSchemeTask:(nonnull id<WKURLSchemeTask>)urlSchemeTask {

}


- (void)userContentController:(WKUserContentController *)userContentController didReceiveScriptMessage:(WKScriptMessage *)message {
    if (self.webView) {
        NSDictionary* dict = (NSDictionary*) message.body;
        NSData* data = [NSJSONSerialization dataWithJSONObject:dict options:NSJSONWritingPrettyPrinted error:nil];
        
        const char* bytes = static_cast<const char*>([data bytes]);
        size_t len = [data length];

        nlohmann::json json = nlohmann::json::parse(bytes, bytes + len, /*cb*/nullptr, /*allow_exceptions*/false);
        
        bool isSK_IPC_call = json.contains("isSK_IPC_call");
        if (isSK_IPC_call) {
            SK_Communication_Config config{self.tag, SK_Communication_Packet_Type::sk_comm_pt_ipc, &json};
            
            SK_WebView* webview = static_cast<SK_WebView*>(self.webView);
            self.skg->onCommunicationRequest(config, [&, webview](const SK_String& ipcResponseData) {
                SK_String data = "sk_api.ipc.handleIncoming(" + ipcResponseData + ")";
                webview->evaluateScript(data.c_str(), NULL);
            }, NULL);
        }
    }
}


@end


@implementation SK_Webview_MacOS_Delegate

- (void)webView:(WKWebView *)webView runOpenPanelWithParameters:(WKOpenPanelParameters *)parameters initiatedByFrame:(WKFrameInfo *)frame completionHandler:(void (^)(NSArray<NSURL *> * _Nullable))completionHandler {
    // Ensure the window handle is valid
    if (!self.windowHandle) {
        NSLog(@"Error: windowHandle is nil.");
        completionHandler(nil); // Handle the error by calling the completion handler with nil
        return;
    }

    // Create an NSOpenPanel
    NSOpenPanel* openPanel = [NSOpenPanel openPanel];

    // Configure the NSOpenPanel based on the WKOpenPanelParameters
    openPanel.allowsMultipleSelection = parameters.allowsMultipleSelection; // Allow multiple files if requested
    openPanel.canChooseFiles = YES; // Always allow files
    openPanel.canChooseDirectories = parameters.allowsDirectories; // Allow directories if requested

    // Present the file dialog
    [openPanel beginSheetModalForWindow:self.windowHandle completionHandler:^(NSInteger result) {
        if (result == NSModalResponseOK) {
            // Pass the selected file(s) to the completion handler
            completionHandler(openPanel.URLs);
        } else {
            // User canceled the dialog
            completionHandler(nil);
        }
    }];
}


/*- (void)webView:(WKWebView *)webView didFinishNavigation:(WKNavigation *)navigation {
    NSLog(@"Page has finished loading!");
    // Perform actions after the page has loaded
}*/

@end

@implementation SK_WebView_MacOS

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)willOpenMenu:(NSMenu *)menu withEvent:(NSEvent *)event {
    SK_WebView* sk_webview_parent = static_cast<SK_WebView*>(self.sk_webview_parent);
    if (!sk_webview_parent) return;

    if (!sk_webview_parent->debugEnabled) [menu removeAllItems];
}

- (void)didCloseMenu:(NSMenu *)menu withEvent:(NSEvent *)event {
    SK_WebView* sk_webview_parent = static_cast<SK_WebView*>(self.sk_webview_parent);
    if (!sk_webview_parent) return;

    if (!sk_webview_parent->debugEnabled) [super didCloseMenu:menu withEvent:event];
}

- (void)keyDown:(NSEvent *)event {
    SK_WebView* sk_webview_parent = static_cast<SK_WebView*>(self.sk_webview_parent);
    if (!sk_webview_parent) { [super keyDown:event]; return; }

    UInt16 code = event.keyCode; // F12 == 111
    NSEventModifierFlags flags =
        (event.modifierFlags & NSEventModifierFlagDeviceIndependentFlagsMask);

    const NSEventModifierFlags want = (NSEventModifierFlagCommand | NSEventModifierFlagShift);

    if (code == 111 && (flags & want) == want) {
        sk_webview_parent->tryActivateDebug();
        return; // handled
    }

    [super keyDown:event];
}


@end



BEGIN_SK_NAMESPACE

// ---------------------------------------------------------------------------
// shutdown() — safe to call repeatedly; second+ calls are no-ops.
// ---------------------------------------------------------------------------
void SK_WebView::shutdown() {
    if (_isShutdown) return;
    _isShutdown = true;

    if (!webview) return;

    // All WKWebView teardown must happen on the main thread.
    auto doTeardown = [this]() {
        // Immediately nil the handler back-pointers so any pending URL scheme
        // task cannot reach this already-dying C++ object.
        messageHandler.webView = nil;
        messageHandler.skg = nil;
        urlHandler.webView = nil;
        urlHandler.skg = nil;

        // stopLoading MUST come first, before niling the navigationDelegate.
        // Calling it while the delegate is still set causes WebKit to internally
        // cancel the pending navigation, which stops NavigationState's
        // m_navigationProgressTimer. If we nil the delegate first, that timer
        // stays alive and later fires NavigationState::ref() against a
        // deallocated WKWebView -> objc_loadWeakRetained crash.
        [webview stopLoading];

        // Nil delegates so no callbacks fire during the rest of teardown.
        webview.UIDelegate = nil;
        webview.navigationDelegate = nil;
        webviewDelegate = nil;
        webview.sk_webview_parent = nil;

        [webview.configuration.userContentController removeScriptMessageHandlerForName:@"SK_IPC_Handler"];
        [webview.configuration.userContentController removeAllUserScripts];

        [webview removeFromSuperview];

        // Quarantine: keep the WKWebView alive permanently so WebKit's
        // internal __weak refs remain valid.
        SK_WebView_MacOS* _toRelease = webview;
        webview = nil;
        SK_QuarantineWKWebView(_toRelease);

        // Invalidate the web page proxy to disconnect the web content
        // process.  Deferred to the NEXT run loop iteration so that
        // _close's internal WebKit cleanup cannot corrupt the heap
        // while our C++ teardown is still running synchronously.  The
        // WKWebView is permanently quarantined so it will still be
        // alive when this fires.
        if (_toRelease) {
          __strong SK_WebView_MacOS* captured = _toRelease;
          dispatch_async(dispatch_get_main_queue(), ^{
            SEL closeSelector = NSSelectorFromString(@"_close");
            if ([captured respondsToSelector:closeSelector]) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
              [captured performSelector:closeSelector];
#pragma clang diagnostic pop
            }
          });
        }
    };

    if (!NSThread.isMainThread) {
        dispatch_sync(dispatch_get_main_queue(), ^{ doTeardown(); });
    } else {
        doTeardown();
    }
}

// ---------------------------------------------------------------------------
// Destructor — delegates to shutdown() if not already called.
// ---------------------------------------------------------------------------
SK_WebView::~SK_WebView(){
    shutdown();
}

void SK_WebView::create(bool offsetWhenDebugging) {
    // WKWebView must be created on the main thread. SK_WebView::create() is
    // called from the module system's native action handler which runs on a
    // background URL-scheme thread. Creating WKWebView off the main thread
    // corrupts PageClientImpl's internal __weak ref, causing commitLayerTree
    // IPC to crash via objc_loadWeakRetained.
    if (!NSThread.isMainThread) {
        dispatch_sync(dispatch_get_main_queue(), ^{
            create(offsetWhenDebugging);
        });
        return;
    }

    // If a WKWebView already exists, tear it down cleanly before creating a
    // new one. Without this, repeated create() calls (e.g. DAW re-shows the
    // window) leak the old WKWebView along with its NavigationState progress
    // timer, which accumulates in CFRunLoop and crashes via objc_loadWeakRetained.
    if (webview) {
        // Already on the main thread — shutdown() handles all teardown.
        shutdown();
        delete readyStateWebResponse;
        readyStateWebResponse = nullptr;
    }

    // Reset the shutdown flag since we are (re-)creating.
    _isShutdown = false;

    readyStateWebResponse = new SK_Communication_Response_Web(SK_Base_URL + "/__sk_sharedBuffer?setReadyState=true");
    readyStateWebResponse->headers["Content-Type"] = "text/text";
    readyStateWebResponse->setAsOK();
    
    NSRect frame = parentWndHandle.contentView.frame;
    
    #if defined(SK_MODE_DEBUG)
        if (offsetWhenDebugging){
            //when we are debugging, we want to expose a bit of the soft backend so that we can right click on it to open its dev tools
            int offset = 32;
            int width = frame.size.width;
            width = width - offset;
            frame.origin.x = offset;
            frame.size.width = width;
        }
    #endif
    
    // Create WKWebViewConfiguration and set preferences
    WKWebViewConfiguration* config = [[WKWebViewConfiguration alloc] init];
    WKPreferences* preferences = [[WKPreferences alloc] init];

    // "developerExtrasEnabled" is a private KVC key removed in newer WebKit.
    // WKWebView.inspectable is the public replacement (applied after view creation below).

    // Enable clipboard access — guard private KVC keys with respondsToSelector:
    // so a future removal cannot cause EXC_BAD_ACCESS inside NSKeyValueAccessorIsEqual.
    if ([preferences respondsToSelector:NSSelectorFromString(@"setDOMPasteAllowed:")])
        [preferences setValue:@YES forKey:@"DOMPasteAllowed"];
    if ([preferences respondsToSelector:NSSelectorFromString(@"setJavaScriptCanAccessClipboard:")])
        [preferences setValue:@YES forKey:@"javaScriptCanAccessClipboard"];

    preferences.javaScriptEnabled = YES;
    
    config.preferences = preferences;
    
    // Create an instance of the Objective-C message handler
    messageHandler = [[SK_WebView_URLSchemeHandler alloc] init];
    messageHandler.tag = tag;
    messageHandler.webView = this;
    messageHandler.skg = skg;
    [config.userContentController addScriptMessageHandler:messageHandler name:@"SK_IPC_Handler"];

    // Register a custom URL scheme handler (for request interception)
    urlHandler = [[SK_WebView_URLSchemeHandler alloc] init];
    urlHandler.tag = tag;
    urlHandler.webView = this;
    urlHandler.skg = skg;
    [config setURLSchemeHandler:urlHandler forURLScheme: @"sk"];

    [config.userContentController  addUserScript:[[WKUserScript alloc] initWithSource:
                                 @"function __SK_IPC_Send(opt) { webkit.messageHandlers.SK_IPC_Handler.postMessage(opt); }"
                                 injectionTime:WKUserScriptInjectionTimeAtDocumentStart
                                 forMainFrameOnly:YES]];
    
    // Create the WKWebView
    webview = [[SK_WebView_MacOS  alloc] initWithFrame:frame configuration:config];
    webview.sk_webview_parent = this;
   
    
    webviewDelegate = [[SK_Webview_MacOS_Delegate alloc] init];
    webviewDelegate.windowHandle = parentWndHandle;
    [webview setUIDelegate:webviewDelegate];
    
    webview.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [webview setValue:@NO forKey:@"drawsBackground"];

    // Enable dev tools using the public API — must be set on the view instance.
    if (@available(macOS 13.3, *)) {
        webview.inspectable = debugEnabled ? YES : NO;
    }
    

    // Disable magnification
    [webview setAllowsMagnification:NO];

    // Add WKWebView to the parent window's content view
    [parentContentView addSubview:webview];

    
    webview.translatesAutoresizingMaskIntoConstraints = false;
    [NSLayoutConstraint activateConstraints:@[
        #if defined(SK_MODE_DEBUG)
            [webview.leadingAnchor constraintEqualToAnchor:parentContentView.leadingAnchor constant:32],
        #else
            [webview.leadingAnchor constraintEqualToAnchor:parentContentView.leadingAnchor],
        #endif
        
        [webview.trailingAnchor constraintEqualToAnchor:parentContentView.trailingAnchor],
        [webview.topAnchor constraintEqualToAnchor:parentContentView.topAnchor],
        [webview.bottomAnchor constraintEqualToAnchor:parentContentView.bottomAnchor]
    ]];
    
    skg->onWebViewReady(parentWnd, static_cast<void*>(webview), false);

    // Navigate to the initial URL
    navigate(currentURL);
    
    notifyReadyToShow();
    
    isReady = true;
}


void SK_WebView::update() {
    // Implement any update logic here if needed
}

void SK_WebView::navigate(const SK_String& url) {
    currentURL = url;
    if (webview == NULL) return;
    
    NSString* urlString = url;

    urlString = [urlString stringByAddingPercentEncodingWithAllowedCharacters: [NSCharacterSet URLQueryAllowedCharacterSet]];

    if (NSURL* _url = [NSURL URLWithString: urlString]) {
        NSMutableURLRequest* request = [NSMutableURLRequest requestWithURL: _url
                                                         cachePolicy: NSURLRequestUseProtocolCachePolicy
                                                     timeoutInterval: 30.0];
        [webview loadRequest: request];
    }
}

void SK_WebView::showDevTools() {
    //seemingly not available on MacOS
}

void SK_WebView::evaluateScript_mainThread(void* _webview, const SK_String& src, SK_WebView_EvaluationComplete_Callback cb) {
    [(__bridge WKWebView*)_webview evaluateJavaScript: src
                 completionHandler:^(id result, NSError *error) {
        if (error) {
            NSLog(@"JS exception: %@ (%@:%@:%@)\n%@",
                   error.userInfo[@"WKJavaScriptExceptionMessage"],
                   error.userInfo[@"WKJavaScriptExceptionSourceURL"],
                   error.userInfo[@"WKJavaScriptExceptionLineNumber"],
                   error.userInfo[@"WKJavaScriptExceptionColumnNumber"],
                   error.userInfo[NSLocalizedDescriptionKey]
            );
            
            /*
            try {
                std::fprintf(stderr, "%s\n", src.data.c_str());
            } catch (const std::exception& e) {
                std::fprintf(stderr, "Could not print script for debugging: %s\n", e.what());
            } catch (...) {
                std::fprintf(stderr, "Could not print script for debugging (unknown exception)\n");
            }
            */
        } else {
            if (cb != nullptr) {
                 if ([result isKindOfClass:[NSString class]]) {
                     SK_String resAsStr = (NSString*)result;
                     cb(resAsStr);
                 } else {
                     cb("");
                 }
            }
        }
    }];
}

void SK_WebView::evaluateScript(const SK_String& src, SK_WebView_EvaluationComplete_Callback cb) {
    if (skg->threadPool->thisFunctionRunningInMainThread()) {
        evaluateScript_mainThread(webview, src, cb);
        return;
    }

    WKWebView* _webview = webview;
    if (!_webview) return;

    // Capture _webview as a strong ObjC ref (ARC keeps it alive) instead of 'this'
    // to avoid use-after-free if SK_WebView is destroyed before the lambda runs.
    skg->threadPool->queueOnMainThread([src, cb, _webview]() {
        [_webview evaluateJavaScript: src
             completionHandler:^(id result, NSError *error) {
            if (error) {
                NSLog(@"JS exception: %@ (%@:%@:%@)\n%@",
                       error.userInfo[@"WKJavaScriptExceptionMessage"],
                       error.userInfo[@"WKJavaScriptExceptionSourceURL"],
                       error.userInfo[@"WKJavaScriptExceptionLineNumber"],
                       error.userInfo[@"WKJavaScriptExceptionColumnNumber"],
                       error.userInfo[NSLocalizedDescriptionKey]
                );
            } else {
                if (cb != nullptr) {
                    if ([result isKindOfClass:[NSString class]]) {
                        SK_String resAsStr = (NSString*)result;
                        cb(resAsStr);
                    } else {
                        cb("");
                    }
                }
            }
        }];
    });
}


void SK_WebView::sendMsgAsJSON_mainThread(void* _webview, const SK_String& src, SK_WebView_EvaluationComplete_Callback cb) {
    evaluateScript_mainThread(_webview, src, cb);
}

void SK_WebView::sendMsgAsJSON(const SK_String& src, SK_WebView_EvaluationComplete_Callback cb) {
    evaluateScript(src, cb);
}


void SK_WebView::addBufferToQueue(size_t size, void* data, const nlohmann::json& metadata = {}){
    if (!sharedBuffersCanBeShared) return;
    
    if (!metadata.is_object()){
        throw std::runtime_error("[SK++    sk_webview_macos.mm -> addBufferToQueue()] Invalid metadata type. Must be object. Is not object.");
    }
    
    tryStartingSharedBuffersTimer();
    
    skg->threadPool->queueOnMainThread([this, size, data, metadata]() {
        sharedBuffersIdx++;
        
        sharedBuffersQueue[sharedBuffersIdx] = new SK_WebView_SharedBuffer(sharedBuffersIdx);
        sharedBuffersQueue[sharedBuffersIdx]->uuid = sharedBuffersIdx;
        sharedBuffersQueue[sharedBuffersIdx]->metadata = metadata;
        sharedBuffersQueue[sharedBuffersIdx]->buffer->data.assign(reinterpret_cast<char*>(data), reinterpret_cast<char*>(data) + size);
    });
}

SK_WebView_SharedBuffer* SK_WebView::getBufferAndRemove(size_t uuid){
    auto it = sharedBuffersQueue.find(uuid);
    if (it == sharedBuffersQueue.end()) {
        return nullptr;
    }

    auto node_handle = sharedBuffersQueue.extract(it);
    
    SK_WebView_SharedBuffer* buffer_ptr = node_handle.mapped();
   
    if (!node_handle) {
        return nullptr;
    }
    
    if (sharedBuffersIdx > 1000000) sharedBuffersIdx = 0;
    
    return buffer_ptr;
}

void SK_WebView::sendSharedBufferOnMainThread(size_t size, void* data, const nlohmann::json& metadata = {}) {
    addBufferToQueue(size, data, metadata);
}

void SK_WebView::sendSharedBuffer(size_t size, void* data, const nlohmann::json& metadata = {}) {
    sendSharedBufferOnMainThread(size, data, metadata);
}

void SK_WebView::tryStartingSharedBuffersTimer(){
    if (sharedBuffersTimer){
        if (!sharedBuffersTimer->isRunning()) {
            sharedBuffersTimer->start();
        }
        return;
    }
    
    sharedBuffersTimer = skg->timerMngr->add(1);
    
    sharedBuffersTimer->on([this](){
        if (sharedBuffersQueue.size() == 0){
            sharedBuffersTimer->stop();
            return;
        }
        
        for (const auto& pair : sharedBuffersQueue) {
            if (!pair.second->busy) {
                pair.second->busy = true;
                SK_String src = "sk_api.dawPluginMngr.fetchQueuedBuffer(" + std::to_string(pair.second->uuid) + "," + pair.second->metadata.dump() + ")";
                
                evaluateScript(src, NULL);
            }
        }
    });
    
    sharedBuffersTimer->start();
}


void SK_WebView::configDebugging() {
    debugActivatorTimer = skg->timerMngr->add(10000);
    debugActivatorTimer->on([&]() {
        debugKeyPressCount = 0;
        enableDebug(false);
        debugActivatorTimer->stop();
    });
    debugActivatorTimer->stop();
};

void SK_WebView::enableDebug(const bool& enable) {
    debugEnabled = enable;
    debugActivatorTimer->reset();
    debugActivatorTimer->stop();
    
    if (@available(macOS 13.3, *)) {
        webview.inspectable = (enable ? YES : NO);
    }
};

void SK_WebView::tryActivateDebug() {
    debugActivatorTimer->reset();
    debugActivatorTimer->start();
    debugKeyPressCount++;

    if (debugKeyPressCount >= 10) {
        debugKeyPressCount = 0;
        enableDebug(!debugEnabled);
    }
};

END_SK_NAMESPACE
