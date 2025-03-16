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
        if (self.skWindow->parent && self.skWindow->isMinimized()) {
            self.skWindow->restore();
        }

        SK::SK_Window_Root::emitWndEvent(self.skWindow, "show",{});
    } else {
        SK::SK_Window_Root::emitWndEvent(self.skWindow, "hide",{});
    }
}

- (void)windowDidBecomeMain:(NSNotification*)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "focus",{});
}

- (void)windowDidResignMain:(NSNotification*)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "blur",{});
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
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "will-resize",{
        {"x", sender.frame.origin.x},
        {"y", sender.frame.origin.y},
        {"width", size.width},
        {"height", size.height},
        {"edge", edge},
    });
    
    return frameSize; // Return the new size
}

- (void)windowDidResize:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "resized",{});
}

- (void)windowWillMove:(NSNotification *)notification {
    NSWindow* window = notification.object;
    NSSize size = [[window contentView] frame].size;
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "will-move",{
        {"x", window.frame.origin.x},
        {"y", window.frame.origin.y},
        {"width", size.width},
        {"height", size.height}
    });
}

- (void)windowDidMove:(NSNotification*)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "move",{});
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "moved",{});
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
    SK::SK_Window* wnd = self.skWindow;
   
    bool shouldClose = false;
    
    if (!self.skWindow->shouldClose_2ndPass){
        SK::SK_Window_Root::emitWndEvent(self.skWindow, "close", {}, [self](nlohmann::json response){
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
    
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "closed",{});
    self.skWindow->shouldClose = false;
    
    self.skWindow->isClosed = true;
}

- (void)windowDidResignKey:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "blur",{});
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "focus",{});
}

- (void)windowDidMiniaturize:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "minimize",{});
}

- (void)windowDidDeminiaturize:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "restore",{});
}

- (void)windowDidEnterFullScreen:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "enter-full-screen",{});
}

- (void)windowWillStartLiveResize:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "resize",{});
}

- (BOOL)windowShouldZoom:(NSWindow*)window toFrame:(NSRect)newFrame {
    self.skWindow->isZooming = true;
    return YES;
}

- (void)windowDidEndLiveResize:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "resized",{});
    
    if (self.skWindow->isZooming) {
        if (self.skWindow->isMaximized())
            SK::SK_Window_Root::emitWndEvent(self.skWindow, "maximize",{});
        else
            SK::SK_Window_Root::emitWndEvent(self.skWindow, "unmaximize",{});
        
      self.skWindow->isZooming = false;
    }
}

- (void)windowDidChangeScreen:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "screen-change",{});
}

- (void)windowDidChangeBackingProperties:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "backing-properties-change",{});
}

- (void)windowDidChangeScreenProfile:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "screen-profile-change",{});
}

- (void)windowDidChangeOrderingMode:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "ordering-mode-change",{});
}

- (void)windowDidEndSheet:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "sheet-end",{});
}

- (void)windowWillBeginSheet:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "sheet-begin",{});
}

- (void)windowDidChangeScreenParameters:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "screen-parameters-change",{});
}

- (void)windowDidChangeToolbarVisibility:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "toolbar-visibility-change",{});
}

- (void)windowDidChangeEffectiveAppearance:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "effective-appearance-change",{});
}

- (void)windowDidChangeTitle:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "title-change",{});
}

- (void)windowDidChangeFirstResponder:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "first-responder-change",{});
}

- (void)windowWillEnterFullScreen:(NSNotification*)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "will-enter-full-screen",{});
}

- (void)windowDidFailToEnterFullScreen:(NSWindow*)window {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "failed-enter-full-screen",{});
}

- (void)windowWillExitFullScreen:(NSNotification*)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "will-leave-full-screen",{});
}

- (void)windowDidExitFullScreen:(NSNotification *)notification {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "leave-full-screen",{});
}

- (IBAction)newWindowForTab:(id)sender {
    SK::SK_Window_Root::emitWndEvent(self.skWindow, "new-window-for-tab",{});
}

@end
