#pragma once

#include "../../sk_common.hpp"

#import <AppKit/AppKit.h>

namespace SK {
class SK_Window;
}

@interface SK_Window_MacOS_Delegate : NSObject <NSWindowDelegate>
@property (nonatomic, assign) SK::SK_Window* skWindow; // Pointer to the C++ SK_Window instance
@end
