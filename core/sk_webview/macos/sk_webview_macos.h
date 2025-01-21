#pragma once

#include <iostream>

#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>

class SK_WebView {
public:
    NSWindow *parentWnd;
    
    WKWebView *webview;
    
    void create();
    void update();

    std::string currentURL = "";
    void navigate(std::string url);
private:

};
