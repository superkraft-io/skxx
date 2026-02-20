#pragma once

#include "../../sk_common.hpp"
#include "../../superkraft.hpp"

#import <Foundation/Foundation.h>
#import <WebKit/WebKit.h>
#import <AppKit/AppKit.h>

#define WKJSE(key) error.userInfo[@#key]


using namespace SK;

@class WKContextMenuElementInfo;

@implementation SK_WebView_URLSchemeHandler

- (void)dealloc {
#if defined(SK_MODE_DEBUG)
    NSLog(@"[SK_WebView][DEALLOC] URLSchemeHandler self=%p webView=%p skg=%p", self, self.webView, self.skg);
#endif
}

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
        
        
        SK_Communicaton_Response_Apple res = sharedBuffer->buffer->getWebResponse();
        [urlSchemeTask didReceiveResponse:res.response];
        [urlSchemeTask didReceiveData:res.data];
        [urlSchemeTask didFinish];
        
        delete sharedBuffer;
        
        return;
    }
    
    SK_Communication_Config config{self.tag, SK_Communication_Packet_Type::sk_comm_pt_web, (__bridge void *)urlSchemeTask.request};
    if (!self.skg) return;
    
    self.skg->onCommunicationRequest(config, NULL, [&](SK_Communication_Packet* packet) -> void* {
        if (packet == nullptr){
            return (static_cast<Superkraft*>(self.skg->sk))->comm->packetFromWebRequest(urlSchemeTask.request, config.sender);
        }
        
        SK_Communication_Response_Web* responseObj = static_cast<SK_Communication_Response_Web*>(packet->response());
        SK_Communicaton_Response_Apple res = responseObj->getWebResponse();
        [urlSchemeTask didReceiveResponse:res.response];
        [urlSchemeTask didReceiveData:res.data];
        [urlSchemeTask didFinish];
        
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

- (void)dealloc {
#if defined(SK_MODE_DEBUG)
    NSLog(@"[SK_WebView][DEALLOC] UIDelegate self=%p window=%p", self, self.windowHandle);
#endif
}

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

- (void)dealloc {
#if defined(SK_MODE_DEBUG)
    NSLog(@"[SK_WebView][DEALLOC] WKWebView self=%p parent=%p", self, self.sk_webview_parent);
#endif
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)willOpenMenu:(NSMenu *)menu withEvent:(NSEvent *)event {
    SK_WebView* sk_webview_parent = static_cast<SK_WebView*>(self.sk_webview_parent);
    
    if (!sk_webview_parent->debugEnabled) [menu removeAllItems];
}

- (void)didCloseMenu:(NSMenu *)menu withEvent:(NSEvent *)event {
    SK_WebView* sk_webview_parent = static_cast<SK_WebView*>(self.sk_webview_parent);
    
    if (!sk_webview_parent->debugEnabled) [super didCloseMenu:menu withEvent:event];
}

- (void)keyDown:(NSEvent *)event {
    SK_WebView* sk_webview_parent = static_cast<SK_WebView*>(self.sk_webview_parent);

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

void SK_WebView::shutdown(){
    if (isShuttingDown) return;
    isShuttingDown = true;

    isReady = false;
    sharedBuffersCanBeShared = false;

    if (sharedBuffersTimer) {
        if (sharedBuffersTimerCallbackId != 0) {
            sharedBuffersTimer->off(sharedBuffersTimerCallbackId);
            sharedBuffersTimerCallbackId = 0;
        }
        sharedBuffersTimer->stop();
    }

    if (debugActivatorTimer) {
        if (debugActivatorTimerCallbackId != 0) {
            debugActivatorTimer->off(debugActivatorTimerCallbackId);
            debugActivatorTimerCallbackId = 0;
        }
        debugActivatorTimer->stop();
    }

    WKWebView* localWebView = webview;
    SK_WebView_URLSchemeHandler* localMessageHandler = messageHandler;
    SK_WebView_URLSchemeHandler* localURLHandler = urlHandler;

    // CFRetain immediately — in MRC, this is a bare pointer with no implicit retain.
    // The host may release the parent view hierarchy (removing us from the superview)
    // before teardownBlock runs, dropping the retain count to 0. We must hold our
    // own retain from this point forward, before anything can release the object.
    CFTypeRef drainView       = localWebView      ? CFRetain((__bridge CFTypeRef)localWebView)      : NULL;
    CFTypeRef drainMsgHandler = localMessageHandler ? CFRetain((__bridge CFTypeRef)localMessageHandler) : NULL;
    CFTypeRef drainURLHandler = localURLHandler   ? CFRetain((__bridge CFTypeRef)localURLHandler)   : NULL;

#if defined(SK_MODE_DEBUG)
    NSLog(@"[SK_WebView] dtor begin wk=%p main=%d", localWebView, [NSThread isMainThread]);
#endif

    webview = nil;
    webviewDelegate = nil;
    messageHandler = nil;
    urlHandler = nil;

    if (!localWebView) {
        // Balance any retains taken above before early-returning.
        if (drainMsgHandler) CFRelease(drainMsgHandler);
        if (drainURLHandler) CFRelease(drainURLHandler);
        return;
    }

    auto teardownBlock = ^{
#if defined(SK_MODE_DEBUG)
        NSLog(@"[SK_WebView] dtor teardown wk=%p main=%d", localWebView, [NSThread isMainThread]);
#endif
        if (localMessageHandler) {
            localMessageHandler.webView = nil;
            localMessageHandler.skg = nil;
        }

        if (localURLHandler) {
            localURLHandler.webView = nil;
            localURLHandler.skg = nil;
        }

        localWebView.UIDelegate = nil;
        localWebView.navigationDelegate = nil;
        [localWebView setHidden:YES];

        // Detach from the window so the display link stops and WKWindowVisibility
        // Observer deregisters cleanly (no new window = no resignKey crash).
        [localWebView removeFromSuperview];

        [localWebView.configuration.userContentController removeScriptMessageHandlerForName:@"SK_IPC_Handler"];
        [localWebView.configuration.userContentController removeAllUserScripts];

        // CFRetain was called at shutdown() entry. Release after 3 main-queue hops:
        // each hop is one CFRunLoop pass, which drains any WTF::RunLoop::performWork()
        // source0 callbacks that were already queued when teardown began.
        dispatch_async(dispatch_get_main_queue(), ^{
            dispatch_async(dispatch_get_main_queue(), ^{
                dispatch_async(dispatch_get_main_queue(), ^{
#if defined(SK_MODE_DEBUG)
                    NSLog(@"[SK_WebView] dtor drain complete wk=%p", localWebView);
#endif
                    if (drainView)       CFRelease(drainView);
                    if (drainMsgHandler) CFRelease(drainMsgHandler);
                    if (drainURLHandler) CFRelease(drainURLHandler);
                });
            });
        });
    };

    if ([NSThread isMainThread]) {
        teardownBlock();
    } else {
        dispatch_sync(dispatch_get_main_queue(), teardownBlock);
    }
}

SK_WebView::~SK_WebView(){
    shutdown();
}

void SK_WebView::create(bool offsetWhenDebugging) {
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
#if defined(SK_MODE_DEBUG)
    NSLog(@"[SK_WebView] create wk=%p main=%d", webview, [NSThread isMainThread]);
#endif

    if (@available(macOS 13.3, *)) {
        webview.inspectable = (debugEnabled ? YES : NO);
    }
   
    
    webviewDelegate = [[SK_Webview_MacOS_Delegate alloc] init];
    webviewDelegate.windowHandle = parentWndHandle;
    [webview setUIDelegate:webviewDelegate];
    
    webview.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [webview setWantsLayer:YES];
    webview.layer.backgroundColor = [NSColor clearColor].CGColor;
    

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

    WKWebView* __strong webviewStrong = webview;
    if (!webviewStrong) return;

    SK_String srcCopy = src;
    dispatch_async(dispatch_get_main_queue(), ^{
        evaluateScript_mainThread(webviewStrong, srcCopy, cb);
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
    
    sharedBuffersTimerCallbackId = sharedBuffersTimer->on([this](){
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
    debugActivatorTimerCallbackId = debugActivatorTimer->on([&]() {
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
