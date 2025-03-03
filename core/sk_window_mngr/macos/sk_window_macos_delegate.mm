#pragma once

#include "../../sk_common.hpp"

#import "sk_window_macos_delegate.h"
#import "sk_window_macos.hpp"

@implementation SK_Window_MacOS_Delegate

#pragma mark - NSWindowDelegate Methods

- (BOOL)windowShouldClose:(NSWindow *)sender {
    self.skWindow->emitEvent("close",{});
    return YES; // Allow the window to close
}

- (void)windowWillClose:(NSNotification *)notification {
    self.skWindow->emitEvent("closed",{});
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
    self.skWindow->emitEvent("focus",{});
}

- (void)windowDidResignKey:(NSNotification *)notification {
    self.skWindow->emitEvent("blur",{});
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

- (void)windowDidExitFullScreen:(NSNotification *)notification {
    self.skWindow->emitEvent("leave-full-screen",{});
}

- (void)windowDidMove:(NSNotification *)notification {
    self.skWindow->emitEvent("moved",{});
}

- (void)windowDidResize:(NSNotification *)notification {
    self.skWindow->emitEvent("resized",{});
}

- (NSSize)windowWillResize:(NSWindow *)sender toSize:(NSSize)frameSize {
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

- (void)windowWillStartLiveResize:(NSNotification *)notification {
    self.skWindow->emitEvent("resize",{});
}

- (void)windowDidEndLiveResize:(NSNotification *)notification {
    self.skWindow->emitEvent("resized",{});
}

- (void)windowWillMove:(NSNotification *)notification {
    self.skWindow->emitEvent("will-move ",{});
}

- (void)windowDidChangeScreen:(NSNotification *)notification {
    self.skWindow->emitEvent("screen-change",{});
}

- (void)windowDidChangeBackingProperties:(NSNotification *)notification {
    self.skWindow->emitEvent("backing-properties-change",{});
}

- (void)windowDidChangeOcclusionState:(NSNotification *)notification {
    self.skWindow->emitEvent("occlusion-state-change",{});
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

- (void)windowDidUpdate:(NSNotification *)notification {
    self.skWindow->emitEvent("update",{});
}

- (void)windowDidExpose:(NSNotification *)notification {
    self.skWindow->emitEvent("expose",{});
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

@end
