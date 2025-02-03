#pragma once

#include "sk_webview_macos.h"

BEGIN_SK_NAMESPACE

@interface SK_WebView_URLSchemeHandler : NSObject <WKURLSchemeHandler>
@end

@implementation SK_WebView_URLSchemeHandler
- (void)webView:(WKWebView *)webView startURLSchemeTask:(id <WKURLSchemeTask>)urlSchemeTask {
    SK_Communication_Config config{ "sk.sb", SK_Communication_Packet_Type::sk_comm_pt_web, urlSchemeTask.request };
    SK_Common::onCommunicationRequest(&config, NULL);
}

- (void)webView:(WKWebView *)webView stopURLSchemeTask:(id <WKURLSchemeTask>)urlSchemeTask {
    // Stop handling custom URL scheme requests
}
@end

void SK_WebView::create() {
    NSRect frame = parentWnd.contentView.frame;

    // Create WKWebViewConfiguration and set preferences
    WKWebViewConfiguration* config = [[WKWebViewConfiguration alloc] init];
    WKPreferences* preferences = [[WKPreferences alloc] init];

    // Enable Developer Extras (DevTools)
    [preferences setValue:@YES forKey:@"developerExtrasEnabled"];
    
    // Enable clipboard access
    [preferences setValue:@YES forKey:@"DOMPasteAllowed"];
    [preferences setValue:@YES forKey:@"javaScriptCanAccessClipboard"];

    config.preferences = preferences;

    // Enable WebKit message handler for JavaScript communication
    [config.userContentController addScriptMessageHandler:self name:@"myMessageHandler"];

    // Register a custom URL scheme handler (for request interception)
    SK_WebView_URLSchemeHandler* urlHandler = [[SK_WebView_URLSchemeHandler alloc] init];
    [config setURLSchemeHandler:urlHandler forURLScheme:@"*"];

    // Create the WKWebView
    webview = [[WKWebView alloc] initWithFrame:frame configuration:config];
    webview.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    webview.layer.backgroundColor = [[NSColor redColor] CGColor];

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
    [parentWnd.contentView addSubview:webview];

    // Navigate to the initial URL
    navigate(currentURL);
}

void SK_WebView::update() {
    // Implement any update logic here if needed
}

void SK_WebView::navigate(std::string url) {
    currentURL = url;
    
    if (webview == NULL) return;

    NSURL* _url = [NSURL URLWithString:[NSString stringWithUTF8String:currentURL.c_str()]];
    
    // Log the URL being loaded for debugging
    NSLog(@"Navigating to URL: %@", _url);
    
    NSURLRequest* request = [NSURLRequest requestWithURL:_url];
    
    // Load the request in the web view
    [webview loadRequest:request];
}

void SK_WebView::evaluateScript_mainThread(WKWebView* _webview, const SK_String& src, SK_WebView_EvaluationComplete_Callback cb) {
    [_webview evaluateJavaScript: src
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
    
    SK_Common::threadPool->queueOnMainThread([this, src, cb, _webview]() {
        evaluateScript_mainThread(_webview, src, cb);
    });
}

void SK_WebView::showDevTools() {
    //seemingly not available on MacOS
}

END_SK_NAMESPACE
