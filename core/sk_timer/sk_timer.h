#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
#include <algorithm>
#include <cmath>

#include "../sk_common.hpp"

BEGIN_SK_NAMESPACE


class SK_Timer {
public:
    using Callback = std::function<void()>;
    using CallbackId = uint64_t; // Unique ID for each registered callback

    SK_String id = "SK_Timer";

    explicit SK_Timer(double interval_ms = 0.0) : interval_ms_(interval_ms) {}

    void setInterval(double interval_ms) {
        interval_ms_ = interval_ms < 0.0 ? 0.0 : interval_ms;
    }
    double interval() const noexcept { return interval_ms_; }


    // on(): Registers a callback and returns a unique ID for removal.
    CallbackId on(Callback cb) {
        // Simple, non-thread-safe unique ID generation
        CallbackId new_id = next_callback_id_++;
        callbacks_[new_id] = std::move(cb);
        return new_id;
    }

    // off(): Unregisters the callback with the given ID.
    void off(CallbackId id) {
        callbacks_.erase(id);
    }
    

    // Start: enable without resetting phase.
    void start() noexcept { enabled_ = true; }

    // Stop: disable and reset phase so next start waits a full interval.
    void stop() noexcept { enabled_ = false; accum_ms_ = 0.0; }

    bool isRunning() const noexcept { return enabled_; }

    void reset() noexcept { accum_ms_ = 0.0; }

    // Fires at most once per manager tick (no catch-up bursts).
    void tick(double dt_ms) {
        if (!enabled_ || interval_ms_ <= 0.0 || dt_ms <= 0.0) return;
        accum_ms_ += dt_ms;
        if (accum_ms_ + kEps >= interval_ms_) {
            accum_ms_ -= interval_ms_;
            if (accum_ms_ < 0.0) accum_ms_ = 0.0;

            // --- Updated Callback Execution ---
            // Iterate and execute ALL registered callbacks
            for (auto const& [id, cb] : callbacks_) {
                cb();
            }
            // ----------------------------------
        }
    }

private:
    static constexpr double kEps = 1e-6;
    double interval_ms_ = 0.0;
    double accum_ms_ = 0.0;
    bool   enabled_ = true;

    // --- New private members ---
    std::map<CallbackId, Callback> callbacks_; // Store multiple callbacks
    CallbackId next_callback_id_ = 1; // Counter for unique IDs
    CallbackId single_cb_id_ = 0; // To maintain compatibility with setCallback
};

class SK_TimerMngr {
public:
    using clock = std::chrono::steady_clock;

    // Create, store, and return a pointer to the timer. Interval is optional.
    SK_Timer* add(double interval_ms = 0.0) {
        auto up = std::make_unique<SK_Timer>(interval_ms);
        SK_Timer* raw = up.get();
        timers_.push_back(std::move(up));
        return raw; // valid while the manager owns it
    }

    void remove(const SK_Timer* t) {
        auto it = std::remove_if(timers_.begin(), timers_.end(),
            [&](const std::unique_ptr<SK_Timer>& p) { return p.get() == t; });
        timers_.erase(it, timers_.end());
    }

    // Call this regularly (e.g., once per frame/idle).
    void tick() {
        auto now = clock::now();
        if (!initialized_) { last_ = now; initialized_ = true; return; }

        const double dt_ms =
            std::chrono::duration<double, std::milli>(now - last_).count();
        last_ = now;

        // Snapshot raw pointers so callbacks can add/remove safely.
        std::vector<SK_Timer*> snapshot;
        snapshot.reserve(timers_.size());
        for (auto& t : timers_) snapshot.push_back(t.get());

        for (auto* t : snapshot) if (t) t->tick(dt_ms);
    }

private:
    std::vector<std::unique_ptr<SK_Timer>> timers_;
    clock::time_point last_{};
    bool initialized_ = false;
};
END_SK_NAMESPACE