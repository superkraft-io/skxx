#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>

#import <WebKit/WebKit.h>

#include "../../utils/sk_str_utils.hxx"

#include "sk_window_macos.h"


 void SK_Window::initialize(unsigned int _wndIdx) {
     SK_Window_Root::initialize(_wndIdx);
 }
 
 void SK_Window::create() {
     NSRect frame = NSMakeRect(top, left, width, height);
     window = [[NSWindow alloc] initWithContentRect:frame
                                                    styleMask:(NSWindowStyleMaskTitled |
                                                               NSWindowStyleMaskClosable |
                                                               NSWindowStyleMaskResizable)
                                                      backing:NSBackingStoreBuffered
                                                        defer:NO];
     
    
    
     [window makeKeyAndOrderFront:nil];
     
     applyTo(window);
 }

int SK_Window::applyTo(NSWindow* wnd){
    
    webview.parentWnd = wnd;
    
    // Create a block to call the static function when the window is resized
    resizeObserver = [[NSNotificationCenter defaultCenter] addObserverForName:NSWindowDidResizeNotification
                                                                       object:wnd
                                                                        queue:nil
                                                                   usingBlock:^(NSNotification *notification) {
        // Call the static function to handle the resize logic
        handleWindowResize(notification);
    }];
    
    setTransparent(transparent);
    
    [window setTitle: StringToNSString(title)];
    
    
    createWebView();
    
    return 0;
}

void SK_Window::createWebView() {
    webview.create();
}


void SK_Window::handleWindowResize(NSNotification* notification) {
    webview.update();
}


void SK_Window::setTransparent(bool _transparent){
    transparent = _transparent;
    
    if (transparent){
        [window setBackgroundColor: [NSColor clearColor]];
        [window setOpaque:NO];
    } else {
        [window setOpaque:YES];
        [window setBackgroundColor: NSColor.whiteColor];
    }
}
