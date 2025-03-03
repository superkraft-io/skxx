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
    
        NSView* backgroundPanel;
        NSView* contentView;
    
        NSVisualEffectView* vibrantView;
        
    #endif
    
    bool isFullscreened;

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
            NSRect frame = NSMakeRect(0, 0, int(config["width"]), int(config["height"]));

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
            [contentView setWantsLayer:YES];
            contentView.layer.masksToBounds = YES;
            [wndHandle setContentView:contentView];
        
        
            backgroundPanel = [[NSView alloc] initWithFrame:frame];
            [backgroundPanel setWantsLayer:YES];
            contentView.layer.backgroundColor = [[NSColor clearColor] CGColor];
            [backgroundPanel setTranslatesAutoresizingMaskIntoConstraints:YES];
            [backgroundPanel setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
            [backgroundPanel setAcceptsTouchEvents:NO];
            [contentView addSubview:backgroundPanel];
        
        
            /*
            vibrantView = [[NSVisualEffectView alloc] initWithFrame:[[wndHandle contentView] bounds]];
            vibrantView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
            vibrantView.blendingMode = NSVisualEffectBlendingModeBehindWindow;
            vibrantView.state = NSVisualEffectStateActive;
            [contentView addSubview:vibrantView positioned:NSWindowBelow relativeTo:nil];
             */
            
        
            NSPoint origin = NSMakePoint(config.data["x"], config.data["y"]);
            [wndHandle setFrameOrigin:origin];
        
            // Make the window key and visible
            [wndHandle makeKeyAndOrderFront:nil];

            updateWindowByConfig();
            
           
        
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
            
            if (checkNeedsUpdateAndReset("alwaysOnTop")) {
                //Incomplete
                //[wndHandle setLevel:(config["alwaysOnTop"] ? NSStatusWindowLevel : NSNormalWindowLevel)];
            }
        
            if (checkNeedsUpdateAndReset("movable")) {
                [wndHandle setMovable: config["movable"]];
            }
        
            if (checkNeedsUpdateAndReset("resizable")) {
                setStyle(NSWindowStyleMaskResizable, config["resizable"]);
            }
            
            if (checkNeedsUpdateAndReset("maximizable")) {
                updateZoomButton();
            }

            if (checkNeedsUpdateAndReset("minimizable")) {
                bool minimizable = config["minimizable"];
                setStyle(NSWindowStyleMaskMiniaturizable, minimizable);
                [[wndHandle standardWindowButton:NSWindowMiniaturizeButton] setEnabled:minimizable];
            }
        
            if (checkNeedsUpdateAndReset("closable")) {
                bool closable = config["closable"];
                setStyle(NSWindowStyleMaskClosable, closable);
                [[wndHandle standardWindowButton:NSWindowCloseButton] setEnabled:closable];
            }
        
            if (checkNeedsUpdateAndReset("transparent")) {
                bool transparent = config["transparent"];
                
                if (transparent){
                    [wndHandle setBackgroundColor: [NSColor clearColor]];
                    [wndHandle setOpaque: NO];
                    
                    contentView.layer.backgroundColor = [[NSColor clearColor] CGColor];
                } else {
                    [wndHandle setBackgroundColor: [NSColor windowBackgroundColor]];
                    [wndHandle setOpaque: YES];
                    
                    contentView.layer.backgroundColor = [[NSColor windowBackgroundColor] CGColor];
                }
            }
        
            if (checkNeedsUpdateAndReset("backgroundColor")) {
                backgroundColor = config.data["backgroundColor"];
                [backgroundPanel.layer setBackgroundColor:backgroundColor];
            }
        
            
            if (checkNeedsUpdateAndReset("focusable")) {
                //incomplete
                bool focusable = config["focusable"];
                /*if (focusable) {
                    [wndHandle setIgnoresMouseEvents:NO];
                } else {
                    [wndHandle setIgnoresMouseEvents:YES];
                }*/
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
                
                setStyle(NSWindowStyleMaskTitled, config["frame"]);
                
                if (!hasStyle(NSWindowStyleMaskFullScreen)){
                    if (hasFrame) {
                        [wndHandle setOpaque:!config["transparent"]];
                        
                        [wndHandle setMovableByWindowBackground:NO];
                        [wndHandle setTitlebarAppearsTransparent:NO];
                        [wndHandle setTitleVisibility:NSWindowTitleVisible];
                        [wndHandle setShowsToolbarButton:YES];
                        
                        [wndHandle setStyleMask: NSTitledWindowMask | NSFullSizeContentViewWindowMask];
                        
                        setStyle(NSResizableWindowMask, config["resizable"]);
                        
                        
                        bool isClosable = config["closable"];
                        [[wndHandle standardWindowButton:NSWindowCloseButton] setHidden:false];
                        [[wndHandle standardWindowButton:NSWindowCloseButton] setEnabled:isClosable];
                        setStyle(NSWindowStyleMaskClosable, isClosable);
                        
                        
                        bool isMinimizable = config["minimizable"];
                        [[wndHandle standardWindowButton:NSWindowMiniaturizeButton] setHidden:false];
                        [[wndHandle standardWindowButton:NSWindowMiniaturizeButton] setEnabled:isMinimizable];
                        setStyle(NSWindowStyleMaskMiniaturizable, isMinimizable);
                        
                        
                        bool isMaximizable = config["maximizable"];
                        [[wndHandle standardWindowButton:NSWindowFullScreenButton] setHidden:false];
                        [[wndHandle standardWindowButton:NSWindowFullScreenButton] setEnabled:isMaximizable];
                        
                        [[wndHandle standardWindowButton:NSWindowZoomButton] setHidden:false];
                        [[wndHandle standardWindowButton:NSWindowZoomButton] setEnabled:isMaximizable];
                        
                        updateZoomButton();
                    } else {
                        [wndHandle setOpaque:NO];
                        [wndHandle setStyleMask:NSResizableWindowMask | NSTitledWindowMask | NSFullSizeContentViewWindowMask];
                        [wndHandle setMovableByWindowBackground:YES];
                        [wndHandle setTitlebarAppearsTransparent:YES];
                        [wndHandle setTitleVisibility:NSWindowTitleHidden];
                        [wndHandle setShowsToolbarButton:NO];
                        
                        // Hide standard window buttons
                        [[wndHandle standardWindowButton:NSWindowFullScreenButton] setHidden:YES];
                        [[wndHandle standardWindowButton:NSWindowMiniaturizeButton] setHidden:YES];
                        [[wndHandle standardWindowButton:NSWindowCloseButton] setHidden:YES];
                        [[wndHandle standardWindowButton:NSWindowZoomButton] setHidden:YES];
                    }
                }
            }
        
            if (checkNeedsUpdateAndReset("opacity")) {
                [wndHandle setAlphaValue: config["opacity"]];
            }

        
        
            if (checkNeedsUpdateAndReset("minWidth") || checkNeedsUpdateAndReset("minHeight")) {
                [wndHandle setContentMinSize:NSMakeSize(config["minWidth"], config["minHeight"])];
            }
            
            if (checkNeedsUpdateAndReset("maxWidth") || checkNeedsUpdateAndReset("maxHeight")) {
                int mW = config["maxWidth"];
                int mH = config["maxHeight"];
                NSSize size = NSMakeSize((mW == -1 ? CGFLOAT_MAX : mW), (mH == -1 ? CGFLOAT_MAX : mH));
                [wndHandle setContentMaxSize:size];
            }
           
        
            if (checkNeedsUpdateAndReset("width") || checkNeedsUpdateAndReset("height")) {
                int w = config.data["width"];
                int h = config.data["height"];
                
                NSRect frame = [wndHandle frame];
                frame.size.width = w;
                frame.size.height = h;
                [wndHandle setFrame:frame display:YES animate:NO];
                [wndHandle setContentSize:NSMakeSize(frame.size.width, frame.size.height)];
            }
        
        
            if (!isMaximized()) {
                if (checkNeedsUpdateAndReset("center") && config.data["center"] == true) {
                    NSRect wndRect = [wndHandle frame];

                    float wndWidth  = wndRect.size.width;
                    float wndHeight = wndRect.size.height;

                    if (config.data["minWidth"] > 0 && wndWidth > config.data["minWidth"]) wndWidth = config.data["minWidth"];
                    if (config.data["minHeight"] > 0 && wndHeight > config.data["minHeight"]) wndHeight = config.data["minHeight"];

                    if (config.data["maxWidth"] > 0 && wndWidth > config.data["maxWidth"]) wndWidth = config.data["maxWidth"];
                    if (config.data["maxHeight"] > 0 && wndHeight > config.data["maxHeight"]) wndHeight = config.data["maxHeight"];

                    float scale = config["scale"];
                    wndWidth *= scale;
                    wndHeight *= scale;

                    NSScreen* screen = [wndHandle screen];
                    NSRect screenRect = [screen visibleFrame];
                    int posx = screenRect.size.width / 2 - wndWidth / 2;
                    int posy = screenRect.size.height / 2 - wndHeight / 2;

                    config.data["x"] = posx;
                    config.data["y"] = posy;
                }

                bool needsReposition = false;
                bool needsResize = false;
                if (checkNeedsUpdateAndReset("x") || checkNeedsUpdateAndReset("y")) needsReposition = true;
                if (checkNeedsUpdateAndReset("width") || checkNeedsUpdateAndReset("width")) needsResize = true;
                
                if (needsReposition || needsResize) {
                    NSPoint origin = NSMakePoint(config.data["x"], config.data["y"]);
                    [wndHandle setFrameOrigin:origin];
                }

                if (checkNeedsUpdateAndReset("show")) {
                    //Working upon window creation, but not after
                    if (config["show"]) {
                        [wndHandle makeKeyAndOrderFront:nil];
                    } else {
                        [wndHandle orderOut:nil];
                    }
                }
            }

            

            if (checkNeedsUpdateAndReset("fullscreen")) {
                setFullscreen(config["fullscreen"]);
            }
         
        #endif
    }

    

    
    #ifdef __OBJC__
    
    void setStyle(NSUInteger style, bool activate) {
        NSUInteger styleMask = [wndHandle styleMask];
        
        if (activate) styleMask |= style;
        else styleMask &= ~style;
        
        [wndHandle setStyleMask:styleMask];
    }
    
    bool hasStyle(NSUInteger flag) {
        return [wndHandle styleMask] & flag;
    }
    
    bool isMinimized() { return [wndHandle isMiniaturized]; }
    
    bool isMaximized() {
        NSRect windowFrame = [wndHandle frame];

        NSScreen* screen = [wndHandle screen];
        if (!screen) return false;
        NSRect screenVisibleFrame = [screen visibleFrame];

        return NSEqualRects(windowFrame, screenVisibleFrame);
    }
    
    
    
    void setFullscreen(bool fullscreen) {
        if (isFullscreened == fullscreen) return;
        
        
        isFullscreened = !isFullscreened;
        config.data["fullScreen"] = isFullscreened;
        
        if (config.data["fullscreenable"] ){
            [wndHandle toggleFullScreen:nil];
        } else {
            [wndHandle zoom:nil];
        }
    }
    
    
    void updateZoomButton(){
        bool isMaximizable = config.data["maximizable"];
        bool isFullscreenable = config.data["fullscreenable"];
        
        if (!isMaximizable && !isFullscreenable){
            [[wndHandle standardWindowButton:NSWindowZoomButton] setEnabled: false];
            return;
        }
        
        [[wndHandle standardWindowButton:NSWindowZoomButton] setEnabled: true];
        
        if (isFullscreenable){
            [wndHandle setCollectionBehavior:[wndHandle collectionBehavior] | NSWindowCollectionBehaviorFullScreenPrimary];
            [wndHandle setCollectionBehavior:[wndHandle collectionBehavior] & (~NSWindowCollectionBehaviorFullScreenAuxiliary)];
        } else {
            [wndHandle setCollectionBehavior:[wndHandle collectionBehavior] | NSWindowCollectionBehaviorFullScreenAuxiliary];
            [wndHandle setCollectionBehavior:[wndHandle collectionBehavior] & (~NSWindowCollectionBehaviorFullScreenPrimary)];
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
