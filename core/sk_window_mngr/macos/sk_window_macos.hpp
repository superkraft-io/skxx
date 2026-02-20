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
        __weak NSWindow* wndHandle = nil;
        __weak NSView* contentView = nil;
    
        __strong NSWindow* wndHandle_strong = nil;
        __strong NSView* contentView_strong = nil;
    
        __strong SK_Window_MacOS_Delegate* wndDelegate = nil;
        __strong NSView* backgroundPanel = nil;
        __strong NSVisualEffectView* vibrantView = nil;
    
        __strong id mouseDownToken = nil;
        __weak NSEvent* lastMouseDownEvent = nil;
    
        NSRect savedUserFrame;
    #endif
    
    bool ignoreUpdateByConfig = false;
    bool isFullscreened = false;
    bool isZooming = false;
    bool blockResizing = false;
    
    SK_Window() {
        config.onChanged = [&](const std::string& key) {
            config_updateTracker[key] = true;
            updateWindowByConfig();
        };
        
        ipc->on("isReady", [&](const nlohmann::json& data, SK_Communication_Packet* packet) {
            packet->response()->JSON({ {"isReady", isReady} });
        });
    }

    	

    #ifdef __OBJC__
    void objcTeardown() {
        @autoreleasepool {            // 1) Stop monitors/observers FIRST
            __closed = true;
            isClosed = true;

            /*if (mouseDownToken) {
                mouseDownToken = nil; //causes crash. since it's a __strong reference, ARC should handle it. (But will it handle it???)
            }*/
            
            // 2) Detach delegate before closing to avoid callbacks into half-dead C++.
            if (wndHandle) {
                [wndHandle setDelegate:nil];
            }
            
            
            if (wndDelegate) {
                wndDelegate.skWindow = nil;
                wndDelegate = nil;	
            }	
            
            // 3) Close window (may synchronously fire delegate/callbacks)
            if (!isInMainWindow()) {
                if (wndHandle) {
                    //[wndHandle close];     // no delegate attached now
                    //wndHandle = nil;    //<<< this will cause crash if uncommented. // optional: nil if you’re sure no one uses it later
                }
                backgroundPanel = nil;
                vibrantView = nil;
                contentView = nil;    // uncomment only if you own it; otherwise leave to close
            }
        }
    }
    
    ~SK_Window(){
        if (![NSThread isMainThread]) {
            __block SK_Window* selfPtr = this;
            dispatch_sync(dispatch_get_main_queue(), ^{ selfPtr->objcTeardown(); });
        } else {
            objcTeardown();
        }
    }
    #endif

    void initialize(const unsigned int& _wndIdx) override {
        SK_Window_Root::initialize(_wndIdx);
        windowClassName += "_" + std::to_string(wndIdx);
    }
    
    #ifdef __OBJC__
        void setDelegate() {
            if (![NSThread isMainThread]) {
              __block SK_Window* selfPtr = this;
              dispatch_sync(dispatch_get_main_queue(), ^{ selfPtr->setDelegate(); });
              return;
            }

            wndDelegate =		 [SK_Window_MacOS_Delegate new];   // retained by __strong ivar
            wndDelegate.skWindow = this;                    // OK: assign to C++ pointer
            [wndHandle setDelegate:wndDelegate];            // NSWindow does NOT retain; you do
          }
    #endif
    
    void create() {
        #ifdef __OBJC__
            // Set the window frame
            NSRect frame = NSMakeRect(0, 0, int(config["width"]), int(config["height"]));

            // Define the window style
            NSUInteger styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
        
            // Create the window
            wndHandle_strong = [[NSWindow alloc] initWithContentRect:frame
                                                styleMask:styleMask
                                                  backing:NSBackingStoreBuffered
                                                    defer:NO];
        
            wndHandle = wndHandle_strong;

            // Set the window title
            [wndHandle setTitle: config["title"]];

            // Create and set the delegate
            setDelegate();

            // Create the content view
            contentView_strong = [[NSView alloc] initWithFrame:frame];
            contentView = contentView_strong;
        
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

            
            // Monitor swipe and rotate gestures
            NSEventMask eventMask = NSEventMaskSwipe | NSEventMaskRotate;
            [NSEvent addLocalMonitorForEventsMatchingMask:eventMask handler:^NSEvent *(NSEvent *event) {
                this->handleGestureEvent(event);
                return event;
            }];
                
            updateWindowByConfig();
            
           
        
            // Create the web view
            createWebView();
        #endif
    }

    void createWebView() {
        webview.notifyReadyToShow = [this]() {
            isReady = true;
            emitWndEvent(this, "ready-to-show", {}, true);
        };
        
        #ifdef __OBJC__
            webview.tag = tag;
            webview.parentWndHandle = wndHandle;
            webview.parentContentView = contentView;
            webview.create(config.data["mainWindow"]);
        #endif
    }

    void updateWebView(bool manuallyResizing = false) {
        #ifdef __OBJC__
            NSRect frame = [contentView frame];
            frame.origin.x = 0.0f;
            frame.origin.y = 0.0f;
            frame.size.width = config.data["width"];
            frame.size.height = config.data["height"];
        
            [NSAnimationContext beginGrouping];
            [[NSAnimationContext currentContext] setDuration:0];
        
            [webview.webview setFrame:frame];
            [NSAnimationContext endGrouping];
            
            // 4. Force immediate update
            [webview.webview setNeedsDisplay:YES];
            [webview.webview displayIfNeeded];
        #endif
    }
   

    void updateWindowByConfig() {
        #ifdef __OBJC__
            if (wndHandle == NULL || ignoreUpdateByConfig == true) return;
        
        
            if (checkNeedsUpdateAndReset("title")) [wndHandle setTitle: config["title"]];
            
            if (checkNeedsUpdateAndReset("alwaysOnTop")) {
                //Incomplete
                //[wndHandle setLevel:(config["alwaysOnTop"] ? NSStatusWindowLevel : NSNormalWindowLevel)];
            }
        
            if (checkNeedsUpdateAndReset("movable")) {
                [wndHandle setMovable: config.data["movable"]];
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
                int mW = config["minWidth"];
                int mH = config["minHeight"];
                if (mW != 0 || mH != 0){
                    NSSize size = NSMakeSize(mW, mH);
                    [wndHandle setContentMinSize:size];
                }
            }
            
            if (checkNeedsUpdateAndReset("maxWidth") || checkNeedsUpdateAndReset("maxHeight")) {
                int mW = config["maxWidth"];
                int mH = config["maxHeight"];
                if (mW != -1 || mH != -1){
                    NSSize size = NSMakeSize((mW == -1 ? CGFLOAT_MAX : mW), (mH == -1 ? CGFLOAT_MAX : mH));
                    [wndHandle setContentMaxSize:size];
                }
            }
           
        
            if (checkNeedsUpdateAndReset("width") || checkNeedsUpdateAndReset("height")) {
                //dispatch_async(dispatch_get_main_queue(), ^{
                    int w = config.data["width"];
                    int h = config.data["height"];
                    
                    NSRect frame = [wndHandle frame];
                    frame.size.width = w;
                    frame.size.height = h;
                
                
                    __weak NSWindow* _wndHandle = wndHandle;
                    __weak NSView* _contentView = contentView;
                
                
                    __weak NSView* _contentViewParent = [contentView superview];
                    NSRect _contentViewParent_frame = [_contentViewParent frame];
                
    
                    //In some cases the NSView which our webview is created added to is not the first layer of our NSWindow.
                    //This is especially true for some DAW plugin windows.
                    //This will cause our webview to be smaller than expected because our frame size does not account for any potential X or Y pos offsets.
                    //To handle this, we callback to our project class (SK_Project) if it exists and handle the frame thre
                
                    bool bypass = false;
                
                    if (skg){
                        if (skg->onBeforeWndResize){
                            SK_Point size = skg->onBeforeWndResize(this);
                            
                            if (size.x == -2) bypass = true;
                            
                            if (size.x > -1) frame.size.width = size.x;
                            if (size.y > -1) frame.size.height = size.y;
                        }
                    }
                   
                    if (!bypass){
                        ignoreUpdateByConfig = true;
                        if (!isInMainWindow()) [wndHandle setFrame:frame display:YES animate:NO];
                        //[wndHandle setContentSize:frame.size];
                        
                        frame.origin.x = 0;
                        frame.origin.y = 0;
                        [contentView setFrame: frame];
                        
                        ignoreUpdateByConfig = false;
                    }
                //});
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
                    if (!isInMainWindow()){
                        ignoreUpdateByConfig = true;
                        [wndHandle setFrameOrigin:origin];
                        ignoreUpdateByConfig = false;
                    }
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
            NSUInteger originalStyleMask = [wndHandle styleMask];
            
            NSUInteger newMask;
            
            if (activate) {
                newMask = originalStyleMask |= style;
            } else {
                newMask = originalStyleMask & ~style;
            }
            
            [wndHandle setStyleMask:newMask];
        }
        
        bool hasStyle(NSUInteger flag) {
            return [wndHandle styleMask] & flag;
        }
        
        
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
        
        
        
        
        
        void maximize(){
            if (isMaximized()) return;
            
            SK_Window* wnd = this;
            if (isInMainWindow()) wnd = skg->mainWindow;
            
            wnd->savedUserFrame = wndHandle.frame;
            
            wnd->isZooming = true;
            
            NSRect target = wndHandle.screen.visibleFrame;
            [wndHandle setFrame:target display:YES animate:YES];
            
        }
    
        void minimize(){
            [wndHandle miniaturize:nil];
        }
        
        void restore(){
            if (wndHandle.isMiniaturized) [wndHandle deminiaturize:nil];
            
            if (isMaximized()){
                SK_Window* wnd = this;
                if (isInMainWindow()) wnd = skg->mainWindow;
                
                wnd->isZooming = true;
                
                [wndHandle setFrame:wnd->savedUserFrame display:YES animate:YES];
            }
            
            [wndHandle makeKeyAndOrderFront:nil];
        }
        
        bool isMinimized() const {
            return [wndHandle isMiniaturized];
        }
        
        
        void handleGestureEvent(NSEvent* event) {
            switch (event.type) {
                case NSEventTypeSwipe:
                    handleSwipeEvent(event);
                    break;
                case NSEventTypeRotate:
                    handleRotateEvent(event);
                    break;
                default:
                    //NSLog(@"Unhandled event type: %lu", (unsigned long)event.type);
                    break;
            }
        }

        void handleSwipeEvent(NSEvent* event) {
            CGFloat deltaX = event.deltaX;
            CGFloat deltaY = event.deltaY;

            
            SK::SK_String direction = "none";
            CGFloat delta = 0;
            
            if (deltaX > 0) {
                direction = "left";
                delta = deltaX;
            } else if (deltaX < 0) {
                direction = "right";
                delta = deltaX;
            }

            if (deltaY > 0) {
                direction = "down";
                delta = deltaY;
            } else if (deltaY < 0) {
                direction = "up";
                delta = deltaY;
            }
            
            SK::SK_Window_Root::emitWndEvent(this, "swipe",{
                {"direction", direction},
                {"delta", delta}
            }, true);
        }

        void handleRotateEvent(NSEvent* event) {
            CGFloat rotation = event.rotation;
            
            SK::SK_Window_Root::emitWndEvent(this, "rotate-gesture",{
                {"rotation", rotation}
            }, true);
        }
    
        void finalizeCreation(){
            setDelegate();
            
            #ifdef __OBJC__
            mouseDownToken = [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskLeftMouseDown handler:^NSEvent* (NSEvent *e) {
                // Create a strong reference inside the block to ensure the object
                // is not deallocated while the handler is executing
                
                if (!wndHandle) return e;
                if (e.windowNumber != wndHandle.windowNumber) return e;

                // Use strongSelf to access members
                lastMouseDownEvent = e;
                
                return e;
            }];
            #endif
        }
            
        void beginMoveFromStoredMouseDown(NSEvent* event) {
            if (event) {
                [wndHandle performWindowDragWithEvent:event];
                event = nil; // drop after use
            }
        }
    
        
    #endif
    
        void readInfo(const SK_String& attribute, SK_Communication_Response& respondWith) {
            #ifdef __OBJC__
            if (attribute == "isMaximized") {
                respondWith.JSON({ {"value", isMaximized()} });
            }
            else if (attribute == "isFullscreen") {
                respondWith.JSON({ {"value", isFullscreened} });
            }
            #endif
        }
    
        void handleWindowAction(const nlohmann::json& payload) {
            SK_String action = "";
            if (payload.contains("action")) action = SK_String(payload["action"]);
            
            #ifdef __OBJC__
                if (action == "beginMoveWindow") {
                    SK_Window* wnd = this;
                    
                    if (isInMainWindow()) wnd = skg->mainWindow;
                    
                    if (wnd->lastMouseDownEvent) {
                        //[wndHandle performWindowDragWithEvent: lastMouseDown];
                        beginMoveFromStoredMouseDown(wnd->lastMouseDownEvent);
                    }
                }
                else if (action == "close")
                {
                    // Closes (and releases) the window
                    [wndHandle performClose:nil];
                }
                else if (action == "focus")
                {
                    // Bring app & window to front and make it key
                    [NSApp activateIgnoringOtherApps:YES];
                    if (wndHandle.isMiniaturized) [wndHandle deminiaturize:nil];
                    [wndHandle makeKeyAndOrderFront:nil];
                }
                else if (action == "blur")
                {
                    // No direct "blur" on macOS. Best-effort: send window behind others
                    // and resign key status.
                    [wndHandle orderBack:nil];
                    [wndHandle resignKeyWindow];
                    // Optional: deactivate the app if you truly want to give up focus
                    // [NSApp deactivate];
                }
                else if (action == "show")
                {
                    [wndHandle orderFront:nil];          // show without necessarily becoming key
                    [wndHandle makeKeyAndOrderFront:nil];// or ensure key + front
                }
                else if (action == "hide")
                {
                    [wndHandle orderOut:nil]; // hides this window (app stays visible)
                }
                else if (action == "maximize")
                {
                    if (config.data.contains("fullscreenable") && config.data["fullscreenable"] == true){
                        if (!isFullscreened) setFullscreen(true);
                        return;
                    }
                    
                    if (!isMaximized()) maximize();
                }
                else if (action == "unmaximize")
                {
                    if (config.data.contains("fullscreenable") && config.data["fullscreenable"] == true){
                        if (isFullscreened) setFullscreen(false);
                        return;
                    }
                    
                    if (isMaximized()) restore();
                }
                else if (action == "minimize")
                {
                    minimize();
                }
                else if (action == "restore")
                {
                    if (config.data.contains("fullscreenable") && config.data["fullscreenable"] == true){
                        if (isFullscreened) setFullscreen(false);
                        return;
                    }
                    
                    restore();
                }
            #endif
        };
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
