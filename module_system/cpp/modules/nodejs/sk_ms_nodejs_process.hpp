#pragma once

#include "../../../../core/sk_common.hpp"

// --- System Includes ---
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cstdlib>
#include <chrono>
#include <filesystem>
#include <csignal>   // For kill/signals
#include <sys/stat.h>// For umask

// --- Platform Specifics ---
#if defined(SK_OS_windows)
    #include <windows.h>
    #include <psapi.h>
    #include <process.h>
    #include <io.h>
    #define GETPID _getpid
    #define UMASK _umask
#else
    #include <unistd.h>
    #include <sys/resource.h>
    #include <sys/utsname.h>
    #include <sys/types.h>
    extern char** environ;
    #define GETPID getpid
    #define UMASK umask
#endif

BEGIN_SK_NAMESPACE

class SK_Module_process {
private:
    std::chrono::steady_clock::time_point startTime;

public:
    SK_Global* skg;

    SK_Module_process(SK_Global* _skg) {
        skg = _skg;
        startTime = std::chrono::steady_clock::now();
    }

    ~SK_Module_process() {
        skg = nullptr;
    }

    void handleOperation(const SK_String& operation, nlohmann::json& payload, SK_Communication_Response& respondWith) {
        // --- Base OS Operations ---
        if (operation == "getCPUInfo") getCPUInfo(respondWith);
        else if (operation == "getMemoryInfo") getMemoryInfo(respondWith);
        else if (operation == "getMachineTime") getMachineTime(respondWith);
        else if (operation == "getNetworInfo") getNetworInfo(respondWith);
        else if (operation == "getUserInfo") getUserInfo(respondWith);

        // --- Node.js 'process' Replica Operations ---

        // Properties
        else if (operation == "process.env") getProcessEnv(respondWith);
        else if (operation == "process.pid") getProcessPid(respondWith);
        else if (operation == "process.ppid") getProcessPpid(respondWith); // NEW
        else if (operation == "process.cwd") getProcessCwd(respondWith);
        else if (operation == "process.version") getProcessVersion(respondWith);
        else if (operation == "process.versions") getProcessVersions(respondWith); // NEW

        // Metrics
        else if (operation == "process.uptime") getProcessUptime(respondWith);
        else if (operation == "process.memoryUsage") getProcessMemoryUsage(respondWith);
        else if (operation == "process.cpuUsage") getProcessCpuUsage(respondWith); // NEW
        else if (operation == "process.hrtime") getProcessHrtime(payload, respondWith); // NEW

        // Actions
        else if (operation == "process.chdir") setProcessChdir(payload, respondWith);
        else if (operation == "process.exit") doProcessExit(payload, respondWith);
        else if (operation == "process.kill") doProcessKill(payload, respondWith); // NEW
        else if (operation == "process.umask") manageProcessUmask(payload, respondWith); // NEW

        // User/Group IDs (POSIX mainly)
        else if (operation == "process.ids") getProcessIDs(respondWith); // NEW (uid, gid, etc)
    };

    // =========================================================
    // Implementation of New 'process' Methods
    // =========================================================

    // 1. process.pid
    void getProcessPid(SK_Communication_Response& respondWith) {
        respondWith.JSON({ {"pid", GETPID()} });
    }

    // 2. process.ppid (Parent Process ID)
    void getProcessPpid(SK_Communication_Response& respondWith) {
        int ppid = 0;
#if defined(SK_OS_windows)
        // Windows way to get Parent PID is complex (requires Toolhelp32Snapshot)
        // Simulating 0 or implementing complex logic if strictly needed.
        // For now, keeping it safe/simple:
        ppid = 0;
#else
        ppid = getppid();
#endif
        respondWith.JSON({ {"ppid", ppid} });
    }

    // 3. process.env
    void getProcessEnv(SK_Communication_Response& respondWith) {
        nlohmann::json envJson;
#if defined(SK_OS_windows)
        char** env_ptr = _environ;
#else
        char** env_ptr = environ;
#endif
        for (char** current = env_ptr; *current; ++current) {
            std::string entry = *current;
            size_t pos = entry.find('=');
            if (pos != std::string::npos) envJson[entry.substr(0, pos)] = entry.substr(pos + 1);
        }
        respondWith.JSON(envJson);
    }

    // 4. process.cwd()
    void getProcessCwd(SK_Communication_Response& respondWith) {
        try {
            respondWith.JSON({ {"cwd", std::filesystem::current_path().string()} });
        }
        catch (...) { respondWith.error(404, "Failed to get CWD"); }
    }

    // 5. process.chdir(directory)
    void setProcessChdir(nlohmann::json& payload, SK_Communication_Response& respondWith) {
        if (payload.contains("directory")) {
            try {
                std::filesystem::current_path(payload["directory"].get<std::string>());
                respondWith.setAsOK();
            }
            catch (const std::filesystem::filesystem_error& e) { respondWith.error(404, e.what()); }
        }
        else { respondWith.error(404, "Missing directory argument"); }
    }

    // 6. process.exit(code)
    void doProcessExit(nlohmann::json& payload, SK_Communication_Response& respondWith) {
        int code = payload.is_number() ? payload.get<int>() : 0;
        if (payload.is_object() && payload.contains("code")) code = payload["code"].get<int>();
        std::exit(code);
    }

    // 7. process.kill(pid, signal)
    void doProcessKill(nlohmann::json& payload, SK_Communication_Response& respondWith) {
        int pid = payload.contains("pid") ? payload["pid"].get<int>() : GETPID();
        // Signal mapping is complex, defaulting to SIGTERM (15) behavior
        int sig = payload.contains("signal") ? 15 : 15;

#if defined(SK_OS_windows)
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if (hProcess == NULL) { respondWith.error(404, "Process not found"); return; }
        TerminateProcess(hProcess, 1);
        CloseHandle(hProcess);
        respondWith.setAsOK();
#else
        if (kill(pid, sig) == 0) respondWith.setAsOK();
        else respondWith.error(404, "Failed to send signal");
#endif
    }

    // 8. process.cpuUsage()
    // Returns user and system time in microseconds
    void getProcessCpuUsage(SK_Communication_Response& respondWith) {
        long long userTime = 0;
        long long sysTime = 0;

#if defined(SK_OS_windows)
        FILETIME ftCreation, ftExit, ftKernel, ftUser;
        if (GetProcessTimes(GetCurrentProcess(), &ftCreation, &ftExit, &ftKernel, &ftUser)) {
            // FILETIME is 100-nanosecond intervals. Divide by 10 to get microseconds.
            ULARGE_INTEGER uKernel, uUser;
            uKernel.LowPart = ftKernel.dwLowDateTime; uKernel.HighPart = ftKernel.dwHighDateTime;
            uUser.LowPart = ftUser.dwLowDateTime; uUser.HighPart = ftUser.dwHighDateTime;
            sysTime = uKernel.QuadPart / 10;
            userTime = uUser.QuadPart / 10;
        }
#else
        struct rusage usage;
        if (getrusage(RUSAGE_SELF, &usage) == 0) {
            userTime = (usage.ru_utime.tv_sec * 1000000) + usage.ru_utime.tv_usec;
            sysTime = (usage.ru_stime.tv_sec * 1000000) + usage.ru_stime.tv_usec;
        }
#endif

        respondWith.JSON({ {"user", userTime}, {"system", sysTime} });
    }

    // 9. process.memoryUsage()
    void getProcessMemoryUsage(SK_Communication_Response& respondWith) {
        size_t rss = 0;
#if defined(SK_OS_windows)
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            rss = pmc.WorkingSetSize;
        }
#else
        struct rusage usage;
        if (getrusage(RUSAGE_SELF, &usage) == 0) {
            rss = usage.ru_maxrss * 1024; // Linux returns KB
        }
#endif
        respondWith.JSON({ {"rss", rss} });
    }

    // 10. process.hrtime()
    // Returns [seconds, nanoseconds]
    void getProcessHrtime(nlohmann::json& payload, SK_Communication_Response& respondWith) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration) - seconds;

        respondWith.JSON(nlohmann::json::array({ seconds.count(), nanoseconds.count() }));
    }

    // 11. process.umask(mask)
    void manageProcessUmask(nlohmann::json& payload, SK_Communication_Response& respondWith) {
        if (payload.contains("mask")) {
            // Setter (returns old mask)
            int newMask = std::stoi(payload["mask"].get<std::string>(), 0, 8); // parsing octal string if sent as string
            int oldMask = UMASK(newMask);
            respondWith.JSON({ {"value", oldMask} });
        }
        else {
            // Getter (trick: set to current to get it, then set back)
            int currentMask = UMASK(0);
            UMASK(currentMask);
            respondWith.JSON({ {"value", currentMask} });
        }
    }

    // 12. process.ids (getuid, getgid, etc)
    void getProcessIDs(SK_Communication_Response& respondWith) {
        nlohmann::json ids;
#if defined(SK_OS_windows)
        ids["uid"] = -1; ids["gid"] = -1; ids["euid"] = -1; ids["egid"] = -1;
#else
        ids["uid"] = getuid();
        ids["gid"] = getgid();
        ids["euid"] = geteuid();
        ids["egid"] = getegid();
#endif
        respondWith.JSON(ids);
    }

    // 13. process.uptime
    void getProcessUptime(SK_Communication_Response& respondWith) {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - startTime;
        respondWith.JSON({ {"uptime", elapsed.count()} });
    }

    // 14. process.versions
    void getProcessVersions(SK_Communication_Response& respondWith) {
        respondWith.JSON({
            {"node", "0.0.0-fake"},
            {"v8", "0.0.0-fake"},
            {"sk_native", "1.0.0"}
            });
    }

    void getProcessVersion(SK_Communication_Response& respondWith) {
        respondWith.JSON({ {"version", "v1.0.0"} });
    }


    // --- Original Methods Helpers ---
    void getCPUInfo(SK_Communication_Response& respondWith) {
        respondWith.JSON(static_cast<SK_Machine*>(skg->machine)->getCPUInfo());
    };
    void getMemoryInfo(SK_Communication_Response& respondWith) {
        respondWith.JSON(static_cast<SK_Machine*>(skg->machine)->getMemoryInfo());
    };
    void getMachineTime(SK_Communication_Response& respondWith) {
        respondWith.JSON(static_cast<SK_Machine*>(skg->machine)->getMachineType());
    };
    void getNetworInfo(SK_Communication_Response& respondWith) {
        respondWith.JSON(static_cast<SK_Machine*>(skg->machine)->getNetworkInfo());
    };
    void getUserInfo(SK_Communication_Response& respondWith) {
        respondWith.JSON(static_cast<SK_Machine*>(skg->machine)->getUserInfo());
    };
};

END_SK_NAMESPACE