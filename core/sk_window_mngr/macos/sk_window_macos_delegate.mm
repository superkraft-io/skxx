#pragma once

#include "../../sk_common.hpp"

#import "sk_window_macos_delegate.h"
#import "sk_window_macos.hpp"

@implementation SK_Window_MacOS_Delegate

#pragma mark - NSWindowDelegate Methods

- (void)windowDidChangeOcclusionState:(NSNotification*)notification {
    NSWindow* window = notification.object;

    // check occlusion binary flag
    if (window.occlusionState & NSWindowOcclusionStateVisible) {
        /*if (self.skWindow->parent && self.skWindow->isMinimized()) {
            self.skWindow->restore();
        }*/

        if (self.skWindow) self.skWindow->emitWndEvent(self.skWindow, "show",{}, true);
    } else {
        if (self.skWindow) self.skWindow->emitWndEvent(self.skWindow, "hide",{}, true);
    }
}

- (void)windowDidBecomeMain:(NSNotification*)notification {
    self.skWindow->emitWndEvent(self.skWindow, "focus",{}, true);
}

- (void)windowDidResignMain:(NSNotification*)notification {
    self.skWindow->emitWndEvent(self.skWindow, "blur",{}, true);
}

- (NSSize)windowWillResize:(NSWindow *)sender toSize:(NSSize)frameSize {
    NSPoint mouseLocation = [NSEvent mouseLocation]; // Get current mouse location in screen coordinates
    NSRect windowFrame = sender.frame; // Get the window's frame

    
    SK::SK_String edge = "none";
    
    // Define corner regions (e.g., 20x20 pixels)
    CGFloat cornerSize = 20.0;

    // Check if the mouse is near a corner
    BOOL isNearTopLeft = mouseLocation.x < NSMinX(windowFrame) + cornerSize && mouseLocation.y > NSMaxY(windowFrame) - cornerSize;
    BOOL isNearTopRight = mouseLocation.x > NSMaxX(windowFrame) - cornerSize && mouseLocation.y > NSMaxY(windowFrame) - cornerSize;
    BOOL isNearBottomLeft = mouseLocation.x < NSMinX(windowFrame) + cornerSize && mouseLocation.y < NSMinY(windowFrame) + cornerSize;
    BOOL isNearBottomRight = mouseLocation.x > NSMaxX(windowFrame) - cornerSize && mouseLocation.y < NSMinY(windowFrame) + cornerSize;

    // Determine the resizing edge or corner
    if (isNearTopLeft) {
        edge = "top-left";
    } else if (isNearTopRight) {
        edge = "top-right";
    } else if (isNearBottomLeft) {
        edge = "bottom-left";
    } else if (isNearBottomRight) {
        edge = "bottom-right";
    } else {
        // If not near a corner, check edges
        CGFloat leftDistance = mouseLocation.x - NSMinX(windowFrame);
        CGFloat rightDistance = NSMaxX(windowFrame) - mouseLocation.x;
        CGFloat topDistance = NSMaxY(windowFrame) - mouseLocation.y;
        CGFloat bottomDistance = mouseLocation.y - NSMinY(windowFrame);

        if (leftDistance < rightDistance && leftDistance < topDistance && leftDistance < bottomDistance) {
            edge = "left";
        } else if (rightDistance < leftDistance && rightDistance < topDistance && rightDistance < bottomDistance) {
            edge = "right";
        } else if (topDistance < leftDistance && topDistance < rightDistance && topDistance < bottomDistance) {
            edge = "top";
        } else if (bottomDistance < leftDistance && bottomDistance < rightDistance && bottomDistance < topDistance) {
            edge = "bottom";
        } else {
            edge = "unknown";
        }
    }

    NSSize size = [[sender contentView] frame].size;
    self.skWindow->emitWndEvent(self.skWindow, "will-resize",{
        {"x", sender.frame.origin.x},
        {"y", sender.frame.origin.y},
        {"width", size.width},
        {"height", size.height},
        {"edge", edge},
    }, true);
    
    return frameSize; // Return the new size
}

- (void)windowDidResize:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "resized",{}, true);
}

- (void)windowWillMove:(NSNotification *)notification {
    NSWindow* window = notification.object;
    NSSize size = [[window contentView] frame].size;
    self.skWindow->emitWndEvent(self.skWindow, "will-move",{
        {"x", window.frame.origin.x},
        {"y", window.frame.origin.y},
        {"width", size.width},
        {"height", size.height}
    }, true);
}

- (void)windowDidMove:(NSNotification*)notification {
    self.skWindow->emitWndEvent(self.skWindow, "move",{}, true);
    self.skWindow->emitWndEvent(self.skWindow, "moved",{}, true);
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
    SK::SK_Window* wnd = self.skWindow;
   
    bool shouldClose = false;
    
    if (!self.skWindow->shouldClose_2ndPass){
        self.skWindow->emitWndEvent(self.skWindow, "close", {}, false, [self](nlohmann::json response){
            if (response.contains("defaultPrevented") && response["defaultPrevented"] == true) {
                self.skWindow->shouldClose = false;
            }
           
            self.skWindow->shouldClose_2ndPass = true;
            [self.skWindow->wndHandle  performClose:nil];
        });
    } else {
        shouldClose = wnd->shouldClose;
        wnd->shouldClose = true;
        wnd->shouldClose_2ndPass = false;
    }
   
    return shouldClose;
}

- (void)windowWillClose:(NSNotification *)notification {
    if (!self.skWindow->shouldClose) return;
    
    self.skWindow->emitWndEvent(self.skWindow, "closed",{}, true);
    self.skWindow->shouldClose = false;
    
    self.skWindow->isClosed = true;
    
    #if defined(SK_APP_TYPE_app)
        if (self.skWindow->subViews.size() > 0){
            SK::SK_Window_Root* firstSubView = self.skWindow->subViews[0];
            
            if (firstSubView->config.data.contains("mainWindow") && firstSubView->config.data["mainWindow"] == true){
                dispatch_async(dispatch_get_main_queue(), ^{
                    [NSApp terminate:nil];
                });
            }
        }
    #endif
}

- (void)windowDidResignKey:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "blur",{}, true);
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "focus",{}, true);
}

- (void)windowDidMiniaturize:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "minimize",{}, true);
}

- (void)windowDidDeminiaturize:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "restore",{}, true);
}

- (void)windowDidEnterFullScreen:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "enter-full-screen",{}, true);
}

- (void)windowWillStartLiveResize:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "resize",{}, true);
}

- (BOOL)windowShouldZoom:(NSWindow*)window toFrame:(NSRect)newFrame {
    self.skWindow->isZooming = true;
    return YES;
}

- (void)windowDidEndLiveResize:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "resized",{}, true);
    
    if (self.skWindow->isZooming) {
        if (self.skWindow->isMaximized())
            self.skWindow->emitWndEvent(self.skWindow, "maximize",{}, true);
        else
            self.skWindow->emitWndEvent(self.skWindow, "unmaximize",{}, true);
        
      self.skWindow->isZooming = false;
    }
}

- (void)windowDidChangeScreen:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "screen-change",{}, true);
}

- (void)windowDidChangeBackingProperties:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "backing-properties-change",{}, true);
}

- (void)windowDidChangeScreenProfile:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "screen-profile-change",{}, true);
}

- (void)windowDidChangeOrderingMode:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "ordering-mode-change",{}, true);
}

- (void)windowDidEndSheet:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "sheet-end",{}, true);
}

- (void)windowWillBeginSheet:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "sheet-begin",{}, true);
}

- (void)windowDidChangeScreenParameters:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "screen-parameters-change",{}, true);
}

- (void)windowDidChangeToolbarVisibility:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "toolbar-visibility-change",{}, true);
}

- (void)windowDidChangeEffectiveAppearance:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "effective-appearance-change",{}, true);
}

- (void)windowDidChangeTitle:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "title-change",{}, true);
}

- (void)windowDidChangeFirstResponder:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "first-responder-change",{}, true);
}

- (void)windowWillEnterFullScreen:(NSNotification*)notification {
    self.skWindow->emitWndEvent(self.skWindow, "will-enter-full-screen",{}, true);
}

- (void)windowDidFailToEnterFullScreen:(NSWindow*)window {
    self.skWindow->emitWndEvent(self.skWindow, "failed-enter-full-screen",{}, true);
}

- (void)windowWillExitFullScreen:(NSNotification*)notification {
    self.skWindow->emitWndEvent(self.skWindow, "will-leave-full-screen",{}, true);
}

- (void)windowDidExitFullScreen:(NSNotification *)notification {
    self.skWindow->emitWndEvent(self.skWindow, "leave-full-screen",{}, true);
}

- (IBAction)newWindowForTab:(id)sender {
    self.skWindow->emitWndEvent(self.skWindow, "new-window-for-tab",{}, true);
}

@end
