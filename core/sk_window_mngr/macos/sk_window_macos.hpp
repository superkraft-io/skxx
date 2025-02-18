#pragma once

#include "../../sk_common.hpp"

#ifdef __OBJC__
  #import <AppKit/AppKit.h>
#endif

BEGIN_SK_NAMESPACE

class SK_Window : public SK_Window_Root {
public:
    #ifdef __OBJC__
        NSWindow* wndHandle;
    #endif
    //NSView* webViewContainer = nullptr;

    SK_Window() {
        config.onChanged = [&](const std::string& key) {
            config_updateTracker[key] = true;
            updateWindowByConfig();
        };
    }

    ~SK_Window() {
        //if (window) {
            //[window release];
        //}
    }

    void initialize(const unsigned int& _wndIdx) override {
        SK_Window_Root::initialize(_wndIdx);
        windowClassName += "_" + std::to_string(wndIdx);
    }

    void create() {
        /*NSRect frame = NSMakeRect(config["x"], config["y"], config["width"], config["height"]);
        NSUInteger styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;

        window = [[NSWindow alloc] initWithContentRect:frame
                                            styleMask:styleMask
                                              backing:NSBackingStoreBuffered
                                                defer:NO];

        [window setTitle: config["title"]];
        [window setDelegate:(id<NSWindowDelegate>)CFBridgingRelease(this)];
        [window makeKeyAndOrderFront:nil];

        createWebView();*/
    }

    void createWebView() {
        #ifdef __OBJC__
            webViewContainer = [[NSView alloc] initWithFrame:[window contentRectForFrameRect:[window frame]]];
            [window setContentView:webViewContainer];
        #endif
        
        // Initialize WebView here (e.g., using WKWebView)
        // webview.create();
    }

    void update(bool manuallyResizing = false) {
        /*NSRect frame = [window frame];
        frame.size.width = config["width"];
        frame.size.height = config["height"];
        [window setFrame:frame display:YES];

        if (webViewContainer) {
            [webViewContainer setFrame:[window contentRectForFrameRect:frame]];
        }*/
    }

    void updateWindowByConfig() {
        /*if (!window) return;

        if (checkNeedsUpdateAndReset("title")) {
            [window setTitle: config["title"]];
        }

        if (checkNeedsUpdateAndReset("resizable")) {
            NSUInteger styleMask = [window styleMask];
            if (config["resizable"]) {
                styleMask |= NSWindowStyleMaskResizable;
            } else {
                styleMask &= ~NSWindowStyleMaskResizable;
            }
            [window setStyleMask:styleMask];
        }

        if (checkNeedsUpdateAndReset("alwaysOnTop")) {
            [window setLevel:(config["alwaysOnTop"] ? NSStatusWindowLevel : NSNormalWindowLevel)];
        }

        if (checkNeedsUpdateAndReset("backgroundColor")) {
            [window setBackgroundColor:backgroundColor];
        }

        if (checkNeedsUpdateAndReset("opacity")) {
            [window setAlphaValue:config["opacity"]];
        }

        if (checkNeedsUpdateAndReset("closable")) {
            [[window standardWindowButton:NSWindowCloseButton] setEnabled:config["closable"]];
        }

        if (checkNeedsUpdateAndReset("show")) {
            if (config["show"]) {
                [window makeKeyAndOrderFront:nil];
            } else {
                [window orderOut:nil];
            }
        }

        if (checkNeedsUpdateAndReset("fullscreen")) {
            setFullscreen(config["fullscreen"]);
        }*/
    }

    void setFullscreen(bool activate) {
        /*if (!config["fullscreenable"]) return;

        if (activate) {
            [window toggleFullScreen:nil];
        } else {
            [window toggleFullScreen:nil];
        }*/
    }

private:
    bool needsWindowUpdate() {
        for (const auto& pair : config_updateTracker.items()) {
            if (pair.value() == true) {
                return true;
            }
        }
        return false;
    }
};

END_SK_NAMESPACE
