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
    self.skWindow->emitEvent("will-resize",{});
    /*self.skWindow->emitEvent("will-resize",{
        {"newBounds", {
            {"x", frameSize.x},
            {"x", frameSize.x},
            {"width", frameSize.width},
            {"height", frameSize.height},
        }},

        {"details", {}}
    });
     */
    return frameSize; // Return the new size
}

- (void)windowDidResize:(NSNotification *)notification {
    self.skWindow->emitEvent("resized",{});
}

- (void)windowWillMove:(NSNotification *)notification {
    self.skWindow->emitEvent("will-move",{});
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
    self.skWindow->emitEvent("close",{});
    return YES; // Allow the window to close
}

- (void)windowWillClose:(NSNotification *)notification {
    self.skWindow->emitEvent("closed",{});
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

- (void)windowDidMove:(NSNotification *)notification {
    self.skWindow->emitEvent("moved",{});
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
