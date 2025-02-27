#pragma once

#include "../../sk_common.hpp"

#ifdef __OBJC__
  #import <AppKit/AppKit.h>
  #import "sk_window_macos_delegate.h" 
#endif

BEGIN_SK_NAMESPACE

class SK_Window : public SK_Window_Root {
public:
    #ifdef __OBJC__
        NSWindow* wndHandle;
        __strong SK_Window_MacOS_Delegate* wndDelegate;
        NSView* contentView;
    NSVisualEffectView* vibrantView;
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
        
        #ifdef __OBJC__
            wndDelegate = nil;
        #endif
    }

    void initialize(const unsigned int& _wndIdx) override {
        SK_Window_Root::initialize(_wndIdx);
        windowClassName += "_" + std::to_string(wndIdx);
    }

    void create() {
    #ifdef __OBJC__
            // Set the window frame
            NSRect frame = NSMakeRect(int(config["x"]), int(config["y"]), int(config["width"]), int(config["height"]));

            // Define the window style
            NSUInteger styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;

            // Create the window
            wndHandle = [[NSWindow alloc] initWithContentRect:frame
                                                styleMask:styleMask
                                                  backing:NSBackingStoreBuffered
                                                    defer:NO];

            // Set the window title
            [wndHandle setTitle: config["title"]];

            // Create and set the delegate
            wndDelegate = [[SK_Window_MacOS_Delegate alloc] init];
            wndDelegate.skWindow = this;
            [wndHandle setDelegate:wndDelegate];

            // Create the content view
            contentView = [[NSView alloc] initWithFrame:frame];
            [wndHandle setContentView:contentView];

        
            vibrantView = [[NSVisualEffectView alloc] initWithFrame:[[wndHandle contentView] bounds]];
            [vibrantView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
            [contentView addSubview:vibrantView positioned:NSWindowBelow relativeTo:nil];
            
        
            // Make the window key and visible
            [wndHandle makeKeyAndOrderFront:nil];

            // Create the web view
            createWebView();
        #endif
    }

    void createWebView() {
        #ifdef __OBJC__
            //webview.callResize = [&]() { update(); };
            webview.parentHandle = wndHandle;
            webview.create();
            //SK_Common::updateWebViewHWNDListForView(windowClassName);
        #endif
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
        #ifdef __OBJC__
            if (!wndHandle) return;

            if (checkNeedsUpdateAndReset("title")) [wndHandle setTitle: config["title"]];
        
            if (checkNeedsUpdateAndReset("resizable")) {
                NSUInteger styleMask = [wndHandle styleMask];
                if (config["resizable"]) {
                    styleMask |= NSWindowStyleMaskResizable;
                } else {
                    styleMask &= ~NSWindowStyleMaskResizable;
                }
                [wndHandle setStyleMask:styleMask];
            }

            if (checkNeedsUpdateAndReset("alwaysOnTop")) {
                [wndHandle setLevel:(config["alwaysOnTop"] ? NSStatusWindowLevel : NSNormalWindowLevel)];
            }
            
            if (checkNeedsUpdateAndReset("maximizable")) {
                BOOL maximizable = config["maximizable"];
                [[wndHandle standardWindowButton:NSWindowZoomButton] setEnabled:maximizable];
            }

            if (checkNeedsUpdateAndReset("minimizable")) {
                BOOL minimizable = config["minimizable"];
                [[wndHandle standardWindowButton:NSWindowMiniaturizeButton] setEnabled:minimizable];
            }
        
            if (checkNeedsUpdateAndReset("backgroundColor")) {
                backgroundColor = config.data["backgroundColor"];
                
                CGColor* clr = backgroundColor;
                
                [contentView setWantsLayer:YES];
                [contentView.layer setBackgroundColor:clr];
            }
        
            
            if (checkNeedsUpdateAndReset("focusable")) {
                bool focusable = config["focusable"];
                if (focusable) {
                    [wndHandle setIgnoresMouseEvents:NO];
                } else {
                    [wndHandle setIgnoresMouseEvents:YES];
                }
            }

            if (checkNeedsUpdateAndReset("skipTaskbar")) {
                bool skipTaskbar = config["skipTaskbar"];
                if (skipTaskbar) {
                    [wndHandle setLevel:NSStatusWindowLevel];
                } else {
                    [wndHandle setLevel:NSNormalWindowLevel];
                }
            }
                
        
        
            if (checkNeedsUpdateAndReset("frame")) {
                bool hasFrame = config["frame"];
                NSUInteger styleMask = [wndHandle styleMask];
                if (hasFrame) {
                    styleMask |= NSWindowStyleMaskTitled;
                } else {
                    styleMask &= ~NSWindowStyleMaskTitled;
                    UpdateWindowMask(false);
                }
                [wndHandle setStyleMask:styleMask];
            }

            if (checkNeedsUpdateAndReset("thickFrame")) {
                bool thickFrame = config["thickFrame"];
                NSUInteger styleMask = [wndHandle styleMask];
                if (thickFrame) {
                    styleMask |= NSWindowStyleMaskResizable;
                } else {
                    styleMask &= ~NSWindowStyleMaskResizable;
                }
                [wndHandle setStyleMask:styleMask];
            }

            if (checkNeedsUpdateAndReset("opacity")) {
                [wndHandle setAlphaValue:config["opacity"]];
            }

            if (checkNeedsUpdateAndReset("closable")) {
                bool closable = config["closable"];
                [[wndHandle standardWindowButton:NSWindowCloseButton] setEnabled:closable];
            }

           
            if (checkNeedsUpdateAndReset("transparent")) {
                bool transparent = config["transparent"];
                [wndHandle setOpaque: (transparent ? NO : YES)];
                [wndHandle setBackgroundColor:[NSColor clearColor]];
            }
  

            if (checkNeedsUpdateAndReset("show")) {
                if (config["show"]) {
                    [wndHandle makeKeyAndOrderFront:nil];
                } else {
                    [wndHandle orderOut:nil];
                }
            }

            if (checkNeedsUpdateAndReset("fullscreen")) {
                setFullscreen(config["fullscreen"]);
            }
        #endif
    }

    void setFullscreen(bool activate) {
        if (!config["fullscreenable"]) return;

        #ifdef __OBJC__
            if (activate) {
                [wndHandle toggleFullScreen:nil];
            } else {
                [wndHandle toggleFullScreen:nil];
            }
        #endif
    }

    
    #ifdef __OBJC__
    
    bool HasStyleMask(NSUInteger flag) const {
      return [wndHandle styleMask] & flag;
    }
    
    void UpdateWindowMask(bool fullscreen) {

        if (vibrantView) {
            const bool roundedCorners = HasStyleMask(NSWindowStyleMaskTitled);
            const float macos_version = SK_Number(SK_Machine::staticInfo["version"]);
            const bool isModal = config["modal"];
            const bool hasFrame = config["frame"];

            // If the window is modal, its corners are rounded on macOS >= 11 or higher
            // unless the user has explicitly passed noRoundedCorners.
            bool doRoundModal = roundedCorners && macos_version >= 11.f && isModal;
            // If the window is nonmodal, its corners are rounded if it is frameless and
            // the user hasn't passed noRoundedCorners.
            bool doRoundNonModal = roundedCorners && isModal && hasFrame;

            if (doRoundModal || doRoundNonModal) {
                CGFloat radius;
                if (fullscreen) {
                    radius = 0.0f;
                } else if (macos_version >= 11.f) {
                    radius = 9.0f;
                } else {
                    // Smaller corner radius on versions prior to Big Sur.
                    radius = 5.0f;
                }

                CGFloat dimension = 2 * radius + 1;
                NSSize size = NSMakeSize(dimension, dimension);
                NSImage* maskImage = [NSImage imageWithSize:size
                                                  flipped:NO
                                           drawingHandler:^BOOL(NSRect rect) {
                                             NSBezierPath* bezierPath = [NSBezierPath
                                                 bezierPathWithRoundedRect:rect
                                                                   xRadius:radius
                                                                   yRadius:radius];
                                             [[NSColor blackColor] set];
                                             [bezierPath fill];
                                             return YES;
                                           }];

                [maskImage setCapInsets:NSEdgeInsetsMake(radius, radius, radius, radius)];
                [maskImage setResizingMode:NSImageResizingModeStretch];
                [vibrantView setMaskImage:maskImage];
            }
        }
    }
    
    
    
    #endif
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
