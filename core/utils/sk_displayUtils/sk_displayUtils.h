#pragma once

#define NOMINMAX
#include <windows.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <cmath>
#include <functional>
#include <thread>
#include <atomic>
#include <chrono>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "user32.lib")

#include "../../sk_common.hpp"

BEGIN_SK_NAMESPACE

#if defined(SK_OS_windows)
class SK_DisplayUtils {
public:
    using OnHighestFpsChanged = std::function<void(double /*old*/, double /*now*/)>;

    // -------- One-shot queries (same as before) --------
    static inline double getHighestFPS() {
        double maxHz = 0.0;
        enumerateDXGI([&](const std::wstring&, double hz) { if (hz > maxHz) maxHz = hz; });
        return maxHz;
    }
    static inline double getHighestFPSCurrent() {
        double maxHz = 0.0;
        enumerateWin32Current([&](const std::wstring&, double hz) { if (hz > maxHz) maxHz = hz; });
        return maxHz;
    }

    // -------- Polling watcher (no threads) --------
    // Call this once to set up. Then call tick() from your own timer/loop.
    static inline void beginMonitoringHighestFPS(OnHighestFpsChanged cb,
        bool useCurrentNotSupported = false,
        bool fireImmediately = true,
        std::chrono::milliseconds minPollInterval = std::chrono::milliseconds{ 0 })
    {
        s_cb() = std::move(cb);
        s_useCurrent() = useCurrentNotSupported;
        s_minInterval() = minPollInterval;
        s_lastCheck() = Clock::now() - s_minInterval(); // allow immediate check

        const double now = s_useCurrent() ? getHighestFPSCurrent() : getHighestFPS();
        s_lastHz() = now;

        if (fireImmediately && s_cb()) {
            s_cb()(now, now);
        }
    }

    // Optional: stop monitoring (clears callback).
    static inline void endMonitoringHighestFPS() {
        s_cb() = nullptr;
    }

    // Call this from your app’s timer/loop; we’ll rate-limit using minPollInterval.
    static inline void tick() {
        const auto nowT = Clock::now();
        if (nowT - s_lastCheck() < s_minInterval()) return;
        s_lastCheck() = nowT;

        const double oldHz = s_lastHz();
        const double nowHz = s_useCurrent() ? getHighestFPSCurrent() : getHighestFPS();
        if (nowHz != oldHz) {
            s_lastHz() = nowHz;
            if (s_cb()) s_cb()(oldHz, nowHz);
        }
    }

private:
    // ---- Helpers ----
    template<typename F>
    static inline void enumerateDXGI(F&& onHz) {
        using Microsoft::WRL::ComPtr;
        ComPtr<IDXGIFactory6> factory;
        if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))) return;

        for (UINT ai = 0;; ++ai) {
            ComPtr<IDXGIAdapter1> adapter;
            if (factory->EnumAdapters1(ai, &adapter) == DXGI_ERROR_NOT_FOUND) break;

            for (UINT oi = 0;; ++oi) {
                ComPtr<IDXGIOutput> output;
                if (adapter->EnumOutputs(oi, &output) == DXGI_ERROR_NOT_FOUND) break;

                DXGI_OUTPUT_DESC desc{};
                if (FAILED(output->GetDesc(&desc))) continue;

                UINT count = 0;
                if (FAILED(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &count, nullptr)) || count == 0)
                    continue;

                std::vector<DXGI_MODE_DESC> modes(count);
                if (FAILED(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &count, modes.data())))
                    continue;

                std::set<int> mhzSeenDisplay;
                for (const auto& m : modes) {
                    if (!m.RefreshRate.Denominator) continue;
                    const double hz = double(m.RefreshRate.Numerator) / double(m.RefreshRate.Denominator);
                    const int mhz = int(std::lround(hz * 1000.0));
                    if (mhzSeenDisplay.insert(mhz).second) onHz(desc.DeviceName, hz);
                }
            }
        }
    }

    template<typename F>
    static inline void enumerateWin32Current(F&& onCurrentHz) {
        for (DWORD i = 0;; ++i) {
            DISPLAY_DEVICEW dd{}; dd.cb = sizeof(dd);
            if (!EnumDisplayDevicesW(nullptr, i, &dd, 0)) break;
            if (!(dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)) continue;

            DEVMODEW cur{}; cur.dmSize = sizeof(cur);
            if (EnumDisplaySettingsExW(dd.DeviceName, ENUM_CURRENT_SETTINGS, &cur, 0)) {
                const double hz = (cur.dmDisplayFrequency >= 2) ? double(cur.dmDisplayFrequency) : 0.0;
                onCurrentHz(dd.DeviceName, hz);
            }
        }
    }

    // ---- state (function-local statics = header-only safe) ----
    using Clock = std::chrono::steady_clock;
    static inline std::function<void(double, double)>& s_cb() { static std::function<void(double, double)> cb; return cb; }
    static inline bool& s_useCurrent() { static bool b = false; return b; }
    static inline double& s_lastHz() { static double v = 0.0; return v; }
    static inline std::chrono::milliseconds& s_minInterval() { static std::chrono::milliseconds d{ 0 }; return d; }
    static inline Clock::time_point& s_lastCheck() { static Clock::time_point t{ Clock::now() - std::chrono::seconds(3600) }; return t; }
};

#elif defined(SK_OS_macos)
    //macos...
#endif


END_SK_NAMESPACE