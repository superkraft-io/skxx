#pragma once

//#include "sk_webview_macos_v2.h"

#include "../../sk_common.hpp"

#import <Foundation/Foundation.h>
#import <WebKit/WebKit.h>
#import <AppKit/AppKit.h>


NS_ASSUME_NONNULL_BEGIN

using namespace SK;

@interface SK_WebView_URLSchemeHandler : NSObject <WKURLSchemeHandler, WKScriptMessageHandler>
@property (nonatomic, assign) SK_WebView* webView;
@property (nonatomic, assign) SK_String tag;
@end

@implementation SK_WebView_URLSchemeHandler
- (void)webView:(WKWebView *)webView startURLSchemeTask:(id <WKURLSchemeTask>)urlSchemeTask {
    //a bare minimum test
    
    SK_String url = urlSchemeTask.request.URL.absoluteString;
    SK_String path = urlSchemeTask.request.URL.path;
    
    
    SK_Communication_Config config{self.tag, SK_Communication_Packet_Type::sk_comm_pt_web, (__bridge void *)urlSchemeTask.request};
    SK_Global::onCommunicationRequest(&config, NULL, [&](SK_Communication_Packet* packet) {
        if (packet == nullptr){
            return SK_Communication_Packet::packetFromWebRequest(urlSchemeTask.request, config.sender);
        }
        
        SK_Communication_Response_Web* responseObj = static_cast<SK_Communication_Response_Web*>(packet->response());
        SK_Communicaton_Response_Apple res = responseObj->getWebResponse();
        [urlSchemeTask didReceiveResponse:res.response];
        [urlSchemeTask didReceiveData:res.data];
        [urlSchemeTask didFinish];
    });

    return;
}

- (void)webView:(nonnull WKWebView *)webView stopURLSchemeTask:(nonnull id<WKURLSchemeTask>)urlSchemeTask {

}


- (void)userContentController:(WKUserContentController *)userContentController didReceiveScriptMessage:(WKScriptMessage *)message {
    if (self.webView) {
        NSDictionary* dict = (NSDictionary*) message.body;
        NSData* data = [NSJSONSerialization dataWithJSONObject:dict options:NSJSONWritingPrettyPrinted error:nil];
        NSString* jsonString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        //mIWebView->OnMessageFromWebView([jsonString UTF8String]);
        
        SK_String _jsonString = jsonString;
        if (_jsonString.indexOf("getInfo") > -1){
            int x = 0;
        }
        
        nlohmann::json json = nlohmann::json::parse([jsonString UTF8String], nullptr, false);
        
        bool isSK_IPC_call = json.contains("isSK_IPC_call");
        if (isSK_IPC_call) {
            SK_Communication_Config config{self.tag, SK_Communication_Packet_Type::sk_comm_pt_ipc, &json};
            
            SK_WebView* webview = self.webView;
            SK_Global::onCommunicationRequest(&config, [&, webview](const SK_String& ipcResponseData) {
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

NS_ASSUME_NONNULL_END

BEGIN_SK_NAMESPACE

void SK_WebView::create() {
    NSRect frame = parentHandle.contentView.frame;
    //frame.size.width = 100;
    //frame.size.height = 100;
    
    // Create WKWebViewConfiguration and set preferences
    WKWebViewConfiguration* config = [[WKWebViewConfiguration alloc] init];
    WKPreferences* preferences = [[WKPreferences alloc] init];

    // Enable Developer Extras (DevTools)
    [preferences setValue:@YES forKey:@"developerExtrasEnabled"];
    
    // Enable clipboard access
    [preferences setValue:@YES forKey:@"DOMPasteAllowed"];
    [preferences setValue:@YES forKey:@"javaScriptCanAccessClipboard"];

    config.preferences = preferences;
    
    // Create an instance of the Objective-C message handler
    SK_WebView_URLSchemeHandler* messageHandler = [[SK_WebView_URLSchemeHandler alloc] init];
    messageHandler.tag = tag;
    messageHandler.webView = this;
    [config.userContentController addScriptMessageHandler:messageHandler name:@"SK_IPC_Handler"];

    // Register a custom URL scheme handler (for request interception)
    SK_WebView_URLSchemeHandler* urlHandler = [[SK_WebView_URLSchemeHandler alloc] init];
    urlHandler.tag = tag;
    urlHandler.webView = this;
    [config setURLSchemeHandler:urlHandler forURLScheme: @"sk"];

    [config.userContentController  addUserScript:[[WKUserScript alloc] initWithSource:
                                 @"function __SK_IPC_Send(opt) { webkit.messageHandlers.SK_IPC_Handler.postMessage(opt); }"
                                 injectionTime:WKUserScriptInjectionTimeAtDocumentStart
                                 forMainFrameOnly:YES]];
    
    // Create the WKWebView
    webview = [[WKWebView alloc] initWithFrame:frame configuration:config];
    
    webviewDelegate = [[SK_Webview_MacOS_Delegate alloc] init];
    webviewDelegate.windowHandle = parentHandle;
    [webview setUIDelegate:webviewDelegate];
    
    webview.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [webview setValue:@NO forKey:@"drawsBackground"];
    
    // Optionally enable isInspectable for macOS 13.3+
    if (@available(macOS 13.3, *)) {
        @try {
            [webview setValue:@YES forKey:@"isInspectable"];
        } @catch (NSException* exception) {
            NSLog(@"Exception enabling isInspectable: %@", exception);
        }
    }

    // Disable magnification
    [webview setAllowsMagnification:NO];

    // Add WKWebView to the parent window's content view
    [parentHandle.contentView addSubview:webview];

    SK_Global::onWebViewReady(static_cast<void*>(webview), false);

    // Navigate to the initial URL
    navigate(currentURL);
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

void SK_WebView::evaluateScript_mainThread(void* _webview, const SK_String& src, SK_WebView_EvaluationComplete_Callback cb) {
    [(__bridge WKWebView*)_webview evaluateJavaScript: src
                 completionHandler:^(id result, NSError *error) {
        if (error) {
            NSLog(@"Error: %@", error.localizedDescription);
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
    if (SK_Thread_Pool::thisFunctionRunningInMainThread()) {
        evaluateScript_mainThread(webview, src, cb);
        return;
    }

    WKWebView* _webview = webview;
    
    SK_Global::threadPool->queueOnMainThread([this, src, cb, _webview]() {
        evaluateScript_mainThread(_webview, src, cb);
    });
}

void SK_WebView::showDevTools() {
    //seemingly not available on MacOS
}

END_SK_NAMESPACE
