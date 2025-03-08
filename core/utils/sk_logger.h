#pragma once

#if defined(SK_OS_windows)
    // for Windows
#elif defined(SK_OS_apple)
    #ifdef __OBJC__
        #import <Foundation/Foundation.h>

        // Define ANSI escape codes for colors
        #define SK_LOGGER_ANSI_COLOR_RESET   @"\x1b[0m"
        #define SK_LOGGER_ANSI_COLOR_RED     @"\x1b[31m"
        #define SK_LOGGER_ANSI_COLOR_YELLOW  @"\x1b[33m"
        #define SK_LOGGER_ANSI_COLOR_BLUE    @"\x1b[34m"

        // Colored logging macros (for terminal output)
        #define SKLog(fmt, ...) NSLog(fmt, ##__VA_ARGS__)

        #define SKLogWarning(fmt, ...) NSLog(@"%@", [NSString stringWithFormat:@"%@%@%@", SK_LOGGER_ANSI_COLOR_YELLOW, [NSString stringWithFormat:fmt, ##__VA_ARGS__], SK_LOGGER_ANSI_COLOR_RESET])

        #define SKLogError(fmt, ...) NSLog(@"%@", [NSString stringWithFormat:@"%@%@%@", SK_LOGGER_ANSI_COLOR_RED, [NSString stringWithFormat:fmt, ##__VA_ARGS__], SK_LOGGER_ANSI_COLOR_RESET])

        #define SKLogInfo(fmt, ...) NSLog(@"%@", [NSString stringWithFormat:@"%@%@%@", SK_LOGGER_ANSI_COLOR_BLUE, [NSString stringWithFormat:fmt, ##__VA_ARGS__], SK_LOGGER_ANSI_COLOR_RESET])
    #endif
#endif
