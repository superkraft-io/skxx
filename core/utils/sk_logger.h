#pragma once

#if defined(SK_OS_windows)
    #include <windows.h>
    #include <cstdio>
    #include <cstdarg>

    // ANSI escape codes (colors). Note: VS "Output" window ignores colors,
    // but they're useful if you also log to a real console.
    #define SK_LOGGER_ANSI_COLOR_RESET   "\x1b[0m"
    #define SK_LOGGER_ANSI_COLOR_RED     "\x1b[31m"
    #define SK_LOGGER_ANSI_COLOR_YELLOW  "\x1b[33m"
    #define SK_LOGGER_ANSI_COLOR_BLUE    "\x1b[34m"

    // Enable ANSI colors in the console (if present). No-op if not available.
    inline void SKWin_EnableVTColorsOnce() {
        static bool done = false;
        if (done) return;
        done = true;

        HANDLE hOut = GetStdHandle(STD_ERROR_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE || hOut == nullptr) return;

        DWORD mode = 0;
        if (!GetConsoleMode(hOut, &mode)) return;

        // Try to enable virtual terminal processing
        SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }

    // Core printer: formats to a buffer, sends to OutputDebugStringA,
    // and mirrors to stderr (optionally colored) if a console exists.
    inline void SKWin_DebugPrintColored(const char* colorOrNull,
        const char* fmt, va_list ap) {
        char msg[4096];
        _vsnprintf_s(msg, _TRUNCATE, fmt, ap);

        // 1) VS Output window
        char line[4200];
        _snprintf_s(line, _TRUNCATE, "%s\n", msg);
        ::OutputDebugStringA(line);

        // 2) Console (optional): color if requested and VT enabled
        //    This is helpful when running outside VS or if you also want terminal logs.
        if (GetConsoleWindow()) {
            SKWin_EnableVTColorsOnce();
            if (colorOrNull) {
                std::fprintf(stderr, "%s%s%s\n",
                    colorOrNull, msg, SK_LOGGER_ANSI_COLOR_RESET);
            }
            else {
                std::fprintf(stderr, "%s\n", msg);
            }
            std::fflush(stderr);
        }
    }

    inline void SKWin_LogImpl(const char* fmt, ...) {
        va_list ap; va_start(ap, fmt);
        SKWin_DebugPrintColored(nullptr, fmt, ap);
        va_end(ap);
    }

    inline void SKWin_LogWarnImpl(const char* fmt, ...) {
        va_list ap; va_start(ap, fmt);
        SKWin_DebugPrintColored(SK_LOGGER_ANSI_COLOR_YELLOW, fmt, ap);
        va_end(ap);
    }

    inline void SKWin_LogErrorImpl(const char* fmt, ...) {
        va_list ap; va_start(ap, fmt);
        SKWin_DebugPrintColored(SK_LOGGER_ANSI_COLOR_RED, fmt, ap);
        va_end(ap);
    }

    inline void SKWin_LogInfoImpl(const char* fmt, ...) {
        va_list ap; va_start(ap, fmt);
        SKWin_DebugPrintColored(SK_LOGGER_ANSI_COLOR_BLUE, fmt, ap);
        va_end(ap);
    }

    // Mac-style variadic macros with printf-style formatting.
    // (The '##__VA_ARGS__' swallows the comma when no extra args are provided.)
    #define SKLog(fmt, ...)        SKWin_LogImpl(fmt, ##__VA_ARGS__)
    #define SKLogWarning(fmt, ...) SKWin_LogWarnImpl(fmt, ##__VA_ARGS__)
    #define SKLogError(fmt, ...)   SKWin_LogErrorImpl(fmt, ##__VA_ARGS__)
    #define SKLogInfo(fmt, ...)    SKWin_LogInfoImpl(fmt, ##__VA_ARGS__)
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
