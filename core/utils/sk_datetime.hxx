#pragma once

#include "../sk_common.hxx"

BEGIN_SK_NAMESPACE


class SK_DateTime {
public:
    #if defined(SK_OS_windows)
        static SK_String formatFileTime(FILETIME ft) {
            SYSTEMTIME st;
            // Convert FILETIME to SYSTEMTIME
            if (FileTimeToSystemTime(&ft, &st)) {
                // Convert SYSTEMTIME to tm structure
                std::tm tm = {};
                tm.tm_year = st.wYear - 1900;  // Year since 1900
                tm.tm_mon = st.wMonth - 1;      // Month (0-based)
                tm.tm_mday = st.wDay;
                tm.tm_hour = st.wHour;
                tm.tm_min = st.wMinute;
                tm.tm_sec = st.wSecond;

                // Extract the milliseconds from the FILETIME (100-nanosecond intervals)
                ULARGE_INTEGER ull;
                ull.LowPart = ft.dwLowDateTime;
                ull.HighPart = ft.dwHighDateTime;

                // Convert to milliseconds (100-nanosecond intervals to milliseconds)
                unsigned long long ms = ull.QuadPart / 10000 % 1000;

                // Format the time as a string
                std::ostringstream oss;
                oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

                // Append milliseconds and "Z" for UTC
                oss << "." << std::setw(3) << std::setfill('0') << ms << "Z";

                return oss.str();
            }
            else {
                return "Invalid time";  // If conversion fails
            }
        }
    #elif SK_OS == macos || SK_OS == ios
    #endif
};


END_SK_NAMESPACE