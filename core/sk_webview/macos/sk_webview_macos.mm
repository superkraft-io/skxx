#pragma once

#include <iostream>

#import <WebKit/WebKit.h>

#include "../../utils/sk_str_utils.hxx"
#include "sk_webview_macos.h"

void SK_WebView::create() {
    NSRect frame = parentWnd.contentView.frame;

    // Create WKWebViewConfiguration and set preferences
    WKWebViewConfiguration* config = [[WKWebViewConfiguration alloc] init];
    WKPreferences* preferences = [[WKPreferences alloc] init];

    [preferences setValue:@YES forKey:@"developerExtrasEnabled"];
    [preferences setValue:@YES forKey:@"DOMPasteAllowed"];
    [preferences setValue:@YES forKey:@"javaScriptCanAccessClipboard"];

    config.preferences = preferences;

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

    //[webview setValue:@(NO) forKey:@"drawsBackground"];
    [webview setAllowsMagnification:NO];

    // Add WKWebView to the parent window's content view
    [parentWnd.contentView addSubview:webview];
    
    

    // Navigate to the initial URL
    navigate(currentURL);
}

void SK_WebView::update() {
    // Implement any update logic here if needed
    //NSRect newFrame = parentWnd.frame;
    //[webview setFrame:newFrame];
}

void SK_WebView::navigate(std::string url) {
    currentURL = url;
    
    if (webview == NULL) return;

    NSURL* _url = [NSURL URLWithString:StringToNSString(currentURL)];
    
    // Log the URL being loaded for debugging
    NSLog(@"Navigating to URL: %@", _url);
    
    NSURLRequest* request = [NSURLRequest requestWithURL:_url];
    
    // Load the request in the web view
    [webview loadRequest:request];
}
