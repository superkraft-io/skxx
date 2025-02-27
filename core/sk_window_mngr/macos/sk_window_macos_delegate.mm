#pragma once

#include "../../sk_common.hpp"

#import "sk_window_macos_delegate.h"
#import "sk_window_macos.hpp"

@implementation SK_Window_MacOS_Delegate

- (void)windowWillClose:(NSNotification *)notification {
    NSLog(@"Window will close");
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
    NSLog(@"Window became key");
}

- (void)windowDidResize:(NSNotification *)notification {
    NSLog(@"Window resized");
}


- (void)setContentView:(NSView *)contentView {
    NSView *wrapperView = [[NSView alloc] initWithFrame:contentView.frame];
    [wrapperView setWantsLayer:YES];
    wrapperView.layer.masksToBounds = YES;
    wrapperView.layer.cornerRadius = 10.0; // Adjust the corner radius as needed
    [wrapperView addSubview:contentView];
    [super setContentView:wrapperView];
}

@end
