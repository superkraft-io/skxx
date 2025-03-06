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

        self.skWindow->emitEvent("show",{});
    } else {
        self.skWindow->emitEvent("hide",{});
    }
}

- (void)windowDidBecomeMain:(NSNotification*)notification {
    self.skWindow->emitEvent("focus",{});
}

- (void)windowDidResignMain:(NSNotification*)notification {
    self.skWindow->emitEvent("blur",{});
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
    self.skWindow->emitEvent("will-resize",{
        {"x", sender.frame.origin.x},
        {"y", sender.frame.origin.y},
        {"width", size.width},
        {"height", size.height},
        {"edge", edge},
    });
    
    return frameSize; // Return the new size
}

- (void)windowDidResize:(NSNotification *)notification {
    self.skWindow->emitEvent("resized",{});
}

- (void)windowWillMove:(NSNotification *)notification {
    NSWindow* window = notification.object;
    NSSize size = [[window contentView] frame].size;
    self.skWindow->emitEvent("will-move",{
        {"x", window.frame.origin.x},
        {"y", window.frame.origin.y},
        {"width", size.width},
        {"height", size.height}
    });
}

- (void)windowDidMove:(NSNotification*)notification {
    self.skWindow->emitEvent("move",{});
    self.skWindow->emitEvent("moved",{});
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
    SK::SK_Window* skWindow = self.skWindow;
    
    if (!self.skWindow->canClose){
        self.skWindow->emitEvent("close",{}, [skWindow](nlohmann::json response){
            bool isNull = response["returnValue"] == "<null>";
            if (!isNull){
                skWindow->canClose = true;
                [skWindow->wndHandle close];
            }
        });
    }
    
    return self.skWindow->canClose;
}

- (void)windowWillClose:(NSNotification *)notification {
    if (!self.skWindow->canClose) return;
    self.skWindow->emitEvent("closed",{});
    self.skWindow->canClose = false;
}

- (void)windowDidResignKey:(NSNotification *)notification {
    self.skWindow->emitEvent("blur",{});
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
    self.skWindow->emitEvent("focus",{});
}

- (void)windowDidMiniaturize:(NSNotification *)notification {
    self.skWindow->emitEvent("minimize",{});
}

- (void)windowDidDeminiaturize:(NSNotification *)notification {
    self.skWindow->emitEvent("restore",{});
}

- (void)windowDidEnterFullScreen:(NSNotification *)notification {
    self.skWindow->emitEvent("enter-full-screen",{});
}

- (void)windowWillStartLiveResize:(NSNotification *)notification {
    self.skWindow->emitEvent("resize",{});
}

- (BOOL)windowShouldZoom:(NSWindow*)window toFrame:(NSRect)newFrame {
    self.skWindow->isZooming = true;
    return YES;
}

- (void)windowDidEndLiveResize:(NSNotification *)notification {
    self.skWindow->emitEvent("resized",{});
    
    if (self.skWindow->isZooming) {
        if (self.skWindow->isMaximized())
            self.skWindow->emitEvent("maximize",{});
        else
            self.skWindow->emitEvent("unmaximize",{});
        
      self.skWindow->isZooming = false;
    }
}

- (void)windowDidChangeScreen:(NSNotification *)notification {
    self.skWindow->emitEvent("screen-change",{});
}

- (void)windowDidChangeBackingProperties:(NSNotification *)notification {
    self.skWindow->emitEvent("backing-properties-change",{});
}

- (void)windowDidChangeScreenProfile:(NSNotification *)notification {
    self.skWindow->emitEvent("screen-profile-change",{});
}

- (void)windowDidChangeOrderingMode:(NSNotification *)notification {
    self.skWindow->emitEvent("ordering-mode-change",{});
}

- (void)windowDidEndSheet:(NSNotification *)notification {
    self.skWindow->emitEvent("sheet-end",{});
}

- (void)windowWillBeginSheet:(NSNotification *)notification {
    self.skWindow->emitEvent("sheet-begin",{});
}

- (void)windowDidChangeScreenParameters:(NSNotification *)notification {
    self.skWindow->emitEvent("screen-parameters-change",{});
}

- (void)windowDidChangeToolbarVisibility:(NSNotification *)notification {
    self.skWindow->emitEvent("toolbar-visibility-change",{});
}

- (void)windowDidChangeEffectiveAppearance:(NSNotification *)notification {
    self.skWindow->emitEvent("effective-appearance-change",{});
}

- (void)windowDidChangeTitle:(NSNotification *)notification {
    self.skWindow->emitEvent("title-change",{});
}

- (void)windowDidChangeFirstResponder:(NSNotification *)notification {
    self.skWindow->emitEvent("first-responder-change",{});
}

- (void)windowWillEnterFullScreen:(NSNotification*)notification {
    self.skWindow->emitEvent("will-enter-full-screen",{});
}

- (void)windowDidFailToEnterFullScreen:(NSWindow*)window {
    self.skWindow->emitEvent("failed-enter-full-screen",{});
}

- (void)windowWillExitFullScreen:(NSNotification*)notification {
    self.skWindow->emitEvent("will-leave-full-screen",{});
}

- (void)windowDidExitFullScreen:(NSNotification *)notification {
    self.skWindow->emitEvent("leave-full-screen",{});
}

- (IBAction)newWindowForTab:(id)sender {
    self.skWindow->emitEvent("new-window-for-tab",{});
}

@end
