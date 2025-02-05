#pragma once

#if defined(SK_OS_windows)
    #include "sk_string_windows.hpp"
#elif defined(SK_OS_macos)
    #include "sk_string_apple.hpp"
#elif defined(SK_OS_ios)
    #include "sk_string_ios.h"
#elif defined(SK_OS_linux)
    #include "sk_string__linux.h"
#elif defined(SK_OS_android)
    #include "sk_string_android.h"
#endif
