#pragma once

#include "../sk_common.hpp"
#include "sk_string/sk_string.h"


#if defined(SK_OS_windows)
    
#else
    struct SK_MemoryInfo {
        uint64_t totalMemory; // in bytes
        uint64_t freeMemory;  // in bytes
        uint64_t usedMemory;  // in bytes
    };
#endif




BEGIN_SK_NAMESPACE

struct SK_CPUInfo {
    std::string model;
    int speed; // in MHz
    long long user;
    long long nice;
    long long sys;
    long long idle;
    long long irq;
};


class SK_Machine {
public:
    static inline std::vector<SK_CPUInfo> cpuInfo;

    static void init() {
        SK_Machine::cpuModel = cpuInfo[0].model;//;SK_Machine::getCPUModel();
        SK_Machine::cpuSpeed = cpuInfo[0].speed;//SK_Machine::getCPUSpeed();
        SK_Machine::staticInfo = SK_Machine::getStaticInfo();
    };

    static inline nlohmann::json staticInfo;

    static nlohmann::json respondError(std::string errorMsg){
        nlohmann::json json {
            {"error", errorMsg}
        };

        return json;
    }


    static bool isBigEndian() {
        uint32_t num = 1;
        uint8_t* bytePtr = reinterpret_cast<uint8_t*>(&num);
        if (bytePtr[0] == 1) return false;
        return true;
    }

    static std::string getCPUArch() {
        return TOSTRING(SK_CPU_ARCH);
    }

    static std::string getMachineType(){
        #if defined(SK_OS_windows)
            SYSTEM_INFO sysinfo;
            GetNativeSystemInfo(&sysinfo);

            switch (sysinfo.wProcessorArchitecture) {
                case PROCESSOR_ARCHITECTURE_AMD64:
                    return "x86_64";  // 64-bit AMD or Intel
                case PROCESSOR_ARCHITECTURE_INTEL:
                    return "i386";    // 32-bit Intel x86
                case PROCESSOR_ARCHITECTURE_ARM:
                    return "arm";     // ARM
                case PROCESSOR_ARCHITECTURE_ARM64:
                    return "arm64";   // ARM64
                case PROCESSOR_ARCHITECTURE_IA64:
                    return "ia64";    // Itanium
                case PROCESSOR_ARCHITECTURE_ALPHA:
                    return "alpha";   // Alpha
                case PROCESSOR_ARCHITECTURE_SHX:
                    return "shx";     // Hitachi SuperH
                case PROCESSOR_ARCHITECTURE_PPC:
                    return "ppc";     // PowerPC
                case PROCESSOR_ARCHITECTURE_MIPS:
                    return "mips";    // MIPS
                case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:
                    return "wow64";   // 32-bit application running on 64-bit Windows
                case PROCESSOR_ARCHITECTURE_ALPHA64:
                    return "alpha64";    // Alpha 64-bit
                case PROCESSOR_ARCHITECTURE_MSIL:
                    return "msil";       // Microsoft Intermediate Language (MSIL)
                case PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64:
                    return "arm32_wow";  // ARM32 application running on 64-bit Windows
                case PROCESSOR_ARCHITECTURE_NEUTRAL:
                    return "neutral";    // ARM64 neutral (not supported in Windows)
                case PROCESSOR_ARCHITECTURE_IA32_ON_ARM64:
                    return "wow_arm64";  // WOW64 on ARM64 (not supported in Windows)
                default:
                    return "Unknown machine type";
            }
        #else
            struct utsname buffer;
            if (uname(&buffer) != 0) {
                return "Unknown machine type";
            }

            return buffer.machine;
        #endif
    }


    static std::string getOSType() {
        // Determine the platform using preprocessor macros

        #if defined(SK_OS_windows)
        // Windows platform
            OSVERSIONINFOEX osvi;
            ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
            osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

            if (GetVersionEx(reinterpret_cast<OSVERSIONINFO*>(&osvi))) {
                switch (osvi.dwPlatformId) {
                case VER_PLATFORM_WIN32_NT:
                    return "Windows_NT";
                case VER_PLATFORM_WIN32_WINDOWS:
                    return "Windows";
                case VER_PLATFORM_WIN32s:
                    return "Windows 3.x";
                default:
                    return "Unknown";
                }
            }
            else {
                return "Unknown";
            }

        #elif defined(SK_OS_macos) || defined(SK_OS_ios)
            struct utsname buffer;
            if (uname(&buffer) == 0) {
                return buffer.sysname;
            }
            else {
                return "Unknown";
            }

        #else
        // Unsupported platform
            return "Unknown";
        #endif
    }

    static std::string getOSPlatform() {
        // Determine the platform using preprocessor macros
        #if defined(_WIN32)
            return "win32";
        #elif defined(__APPLE__)
            return "darwin";
        #elif defined(__linux__)
            return "linux";
        #elif defined(__FreeBSD__)
            return "freebsd";
        #elif defined(__OpenBSD__)
            return "openbsd";
        #elif defined(__NetBSD__)
            return "netbsd";
        #elif defined(__sun) && defined(__SVR4)
            return "sunos";
        #elif defined(_AIX)
            return "aix";
        #elif defined(__ANDROID__)
            return "android";
        #elif defined(__DragonFly__)
            return "dragonfly";
        #else
            return "unknown";
        #endif
    }


    static std::string getOSVersion(bool releaseVersion = true) {
        #if defined(SK_OS_windows)
            RTL_OSVERSIONINFOEXW osvi;
            ZeroMemory(&osvi, sizeof(RTL_OSVERSIONINFOEXW));
            osvi.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);

            // Load ntdll.dll dynamically
            HMODULE hMod = GetModuleHandle("ntdll.dll");
            if (hMod != nullptr) {
                // Get address of RtlGetVersion function
                RtlGetVersionFunc pRtlGetVersion = reinterpret_cast<RtlGetVersionFunc>(GetProcAddress(hMod, "RtlGetVersion"));
                if (pRtlGetVersion != nullptr) {
                    // Call RtlGetVersion to fill osvi structure
                    if (pRtlGetVersion(&osvi) == 0) { // Check for success directly
                        // Construct version string
                        std::string version = std::to_string(osvi.dwMajorVersion) + "." + std::to_string(osvi.dwMinorVersion) + "." + std::to_string(osvi.dwBuildNumber);
                        
                        if (releaseVersion == false){
                            version = "Windows " + std::to_string(osvi.dwMajorVersion);
                            if (osvi.wSuiteMask & VER_SUITE_PERSONAL) version += " Home";
                            else  version += " Pro";
                        }

                        return version;
                    }
                }
            }
            return "unknown";
        #else
            FILE* pipe = popen("/usr/bin/sw_vers -productVersion", "r");
            if (!pipe) return "unknown";

            char buffer[128];
            std::string result = "";
            while (!feof(pipe)) {
                if (fgets(buffer, 128, pipe) != NULL)
                    result += buffer;
            }
            pclose(pipe);

            // Trim newline character if present
            if (!result.empty() && result[result.length() - 1] == '\n')
                result.erase(result.length() - 1);

            return result;
        #endif
    }


    static std::vector<SK_CPUInfo> getCPUInformation() {
        std::vector<SK_CPUInfo> cpus;
        
        #if defined(SK_OS_windows)
            //THIS FUNCTION DOES NOT WORK IN WINDOWS BECAUSE APPARENTLY IT MNUST BE CALLED IN THE PROGRAIM main() FUNCTION
            //IN OTHER WORDS IT MUST BE CALLED AS SOON AS THE PROGRAM STARTS AND THUS THIS FUNCTION CANNOT BE CALLED
            //AT A LATER STAGE OF RUNTIME

            //UPDATE: In iPlug2 this works by adding "std::vector<SK_CPUInfo> cpuInfo = SK::SK_Machine::getCPUInformation();" in the beginning of the file "IPlugAPP_main.cpp"


            HRESULT hres;
            hres = CoInitializeEx(NULL, COINIT_MULTITHREADED);
            if (FAILED(hres)) {
                std::cerr << "Failed to initialize COM library. Error code = 0x" << std::hex << hres << std::endl;
            }

            hres = CoInitializeSecurity(
                NULL,
                -1,                          // COM authentication
                NULL,                        // Authentication services
                NULL,                        // Reserved
                RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
                RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
                NULL,                        // Authentication info
                EOAC_NONE,                   // Additional capabilities 
                NULL                         // Reserved
            );

            if (FAILED(hres)) {
                std::cerr << "Failed to initialize security. Error code = 0x" << std::hex << hres << std::endl;
                CoUninitialize();
                return cpus;
            }

            IWbemLocator* pLoc = NULL;
            hres = CoCreateInstance(
                CLSID_WbemLocator,
                0,
                CLSCTX_INPROC_SERVER,
                IID_IWbemLocator, (LPVOID*)&pLoc);

            if (FAILED(hres)) {
                std::cerr << "Failed to create IWbemLocator object. Error code = 0x" << std::hex << hres << std::endl;
                CoUninitialize();
                return cpus;
            }

            IWbemServices* pSvc = NULL;
            hres = pLoc->ConnectServer(
                _bstr_t(L"ROOT\\CIMV2"),
                NULL,
                NULL,
                0,
                NULL,
                0,
                0,
                &pSvc);

            if (FAILED(hres)) {
                std::cerr << "Could not connect. Error code = 0x" << std::hex << hres << std::endl;
                pLoc->Release();
                CoUninitialize();
                return cpus;
            }

            hres = CoSetProxyBlanket(
                pSvc,
                RPC_C_AUTHN_WINNT,
                RPC_C_AUTHZ_NONE,
                NULL,
                RPC_C_AUTHN_LEVEL_CALL,
                RPC_C_IMP_LEVEL_IMPERSONATE,
                NULL,
                EOAC_NONE
            );

            if (FAILED(hres)) {
                std::cerr << "Could not set proxy blanket. Error code = 0x" << std::hex << hres << std::endl;
                pSvc->Release();
                pLoc->Release();
                CoUninitialize();
                return cpus;
            }

            IEnumWbemClassObject* pEnumerator = NULL;
            hres = pSvc->ExecQuery(
                bstr_t("WQL"),
                bstr_t("SELECT * FROM Win32_Processor"),
                WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                NULL,
                &pEnumerator);

            if (FAILED(hres)) {
                std::cerr << "Query for CPU info failed. Error code = 0x" << std::hex << hres << std::endl;
                pSvc->Release();
                pLoc->Release();
                CoUninitialize();
                return cpus;
            }

            IWbemClassObject* pclsObj = NULL;
            ULONG uReturn = 0;

            while (pEnumerator) {
                HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
                if (0 == uReturn) {
                    break;
                }

                VARIANT vtProp;

                SK_CPUInfo cpu;
                hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
                if (SUCCEEDED(hr)) {
                    cpu.model = _bstr_t(vtProp.bstrVal);
                    VariantClear(&vtProp);
                }

                hr = pclsObj->Get(L"MaxClockSpeed", 0, &vtProp, 0, 0);
                if (SUCCEEDED(hr)) {
                    cpu.speed = vtProp.intVal;
                    VariantClear(&vtProp);
                }

                // Simulated values for times as they are not readily available via WMI
                cpu.user = 0;
                cpu.nice = 0;
                cpu.sys = 0;
                cpu.idle = 0;
                cpu.irq = 0;

                cpus.push_back(cpu);
                pclsObj->Release();
            }

            pSvc->Release();
            pLoc->Release();
            pEnumerator->Release();
            CoUninitialize();
        #elif defined(SK_OS_macos) || defined(SK_OS_ios)
            // Retrieve the CPU model string.
            std::string model;
            #if defined(SK_OS_macos)
                // On macOS, use "machdep.cpu.brand_string"
                size_t size = 0;
                sysctlbyname("machdep.cpu.brand_string", NULL, &size, NULL, 0);
                if (size > 0) {
                    char* brand = new char[size];
                    if (sysctlbyname("machdep.cpu.brand_string", brand, &size, NULL, 0) == 0) {
                        model = brand;
                    }
                    delete[] brand;
                }
            #elif defined(SK_OS_ios)
                // On iOS, "machdep.cpu.brand_string" is not available.
                // Use "hw.machine" which returns the device model identifier.
                size_t size = 0;
                sysctlbyname("hw.machine", NULL, &size, NULL, 0);
                if (size > 0) {
                    char* machine = new char[size];
                    if (sysctlbyname("hw.machine", machine, &size, NULL, 0) == 0) {
                        model = machine;
                    }
                    delete[] machine;
                }
            #endif

            // Retrieve the CPU frequency (in Hz) and convert to MHz.
            uint64_t frequency = 0;
            size_t freqSize = sizeof(frequency);
            if (sysctlbyname("hw.cpufrequency", &frequency, &freqSize, NULL, 0) != 0) {
                frequency = 0;
            }
            int speedMHz = static_cast<int>(frequency / 1000000);

            // Retrieve CPU usage times using host_statistics.
            host_cpu_load_info_data_t cpuLoad;
            mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
            kern_return_t kr = host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO,
                                               reinterpret_cast<host_info_t>(&cpuLoad), &count);
            uint64_t user = 0, sysTime = 0, idle = 0, nice = 0;
            if (kr == KERN_SUCCESS) {
                user   = cpuLoad.cpu_ticks[CPU_STATE_USER];
                sysTime= cpuLoad.cpu_ticks[CPU_STATE_SYSTEM];
                idle   = cpuLoad.cpu_ticks[CPU_STATE_IDLE];
                nice   = cpuLoad.cpu_ticks[CPU_STATE_NICE];
            }

            // Assemble the CPU info.
            SK_CPUInfo cpu;
            cpu.model = model;
            cpu.speed = speedMHz;
            cpu.user  = user;
            cpu.nice  = nice;
            cpu.sys   = sysTime;
            cpu.idle  = idle;
            cpu.irq   = 0; // IRQ not available on macOS/iOS

            cpus.push_back(cpu);
        #endif
        
        return cpus;
    }


    static std::string getHostname() {
        #if defined(SK_OS_windows)
            TCHAR buffer[MAX_COMPUTERNAME_LENGTH + 1];
            DWORD size = sizeof(buffer) / sizeof(buffer[0]);

            if (GetComputerNameEx(ComputerNameDnsHostname, buffer, &size)) {
                return std::string(buffer);
            }
            else {
                return "<unknown>";
            }

        #elif defined(SK_OS_macos) || defined(SK_OS_ios)
            char buffer[256];
            if (gethostname(buffer, sizeof(buffer)) == 0) {
                return std::string(buffer);
            }
            else {
                return "<unknown>";
            }
        #endif
    }

    static nlohmann::json getStaticInfo(){
        nlohmann::json json {
            {"EOL", "\\n"},
            {"endianess", (SK_Machine::isBigEndian() ? "BE" : "LE")},
            {"arch", SK_Machine::getCPUArch()},
            {"machine", SK_Machine::getMachineType()},
            {"platform", SK_Machine::getOSPlatform()},
            {"release", SK_Machine::getOSVersion()}, //OS build
            {"type", SK_Machine::getOSType()},
            {"version", SK_Machine::getOSVersion(false)},

            {"devNull", "/dev/null"},

            { "hostname", SK_Machine::getHostname()},

            { "homedir", SK::SK_Path_Utils::paths["home"] },
            { "tmpdir", SK::SK_Path_Utils::paths["temp"] }
        };

        #if defined(SK_OS_android)
        #elif defined(SK_OS_macos) || defined(SK_OS_ios)
        #elif defined(SK_OS_linux)
        #elif defined(SK_OS_windows)
            json["EOL"] = "\\r\\n";
            json["devNull"] = "\\\\.\\nul";
        #endif
        
        return json;
    }


    static inline std::string cpuModel;
    static std::string getCPUModel() {
        #if defined(SK_OS_windows)
            int cpuInfo[4] = {0};
            char cpuBrandString[49] = { 0 };

            __cpuid(cpuInfo, 0x80000000);
            unsigned int maxExtendedIDs = cpuInfo[0];

            if (maxExtendedIDs >= 0x80000004) {
                __cpuid(reinterpret_cast<int*>(cpuInfo), 0x80000002);
                memcpy(cpuBrandString, cpuInfo, sizeof(cpuInfo));
                __cpuid(reinterpret_cast<int*>(cpuInfo), 0x80000003);
                memcpy(cpuBrandString + 16, cpuInfo, sizeof(cpuInfo));
                __cpuid(reinterpret_cast<int*>(cpuInfo), 0x80000004);
                memcpy(cpuBrandString + 32, cpuInfo, sizeof(cpuInfo));
            }

            return cpuBrandString;

        #elif defined(SK_OS_macos) || defined(SK_OS_ios)

            //Either this...
            
            char buffer[256];
            size_t size = sizeof(buffer);
            sysctlbyname("machdep.cpu.brand_string", buffer, &size, nullptr, 0);
            return std::string(buffer);
            

            //...or this
            /*
            std::ifstream cpuinfo("/proc/cpuinfo");
            std::string line;
            while (std::getline(cpuinfo, line)) {
                if (line.find("model name") != std::string::npos) {
                    size_t colonPos = line.find(':');
                    if (colonPos != std::string::npos) {
                        cpuModel = line.substr(colonPos + 2); // Skip ": "
                    }
                    break;
                }
            }
            cpuinfo.close();
            */

            return "";
        #elif defined(SK_OS_linux)
            std::ifstream cpuInfoFile("/proc/cpuinfo");
            std::string line;
            while (std::getline(cpuInfoFile, line)) {
                if (line.find("model name") != std::string::npos) {
                    std::string::size_type pos = line.find(":");
                    return line.substr(pos + 1);
                }
            }
            return "unknown";

        #else
            return "unsupported";
        #endif
    }

    static inline int cpuSpeed;
    static int getCPUSpeed() {
        //This function needs A LOT of work. Numbers come out wrong on Windows, and in NodeJS the numbers are different for each CPU core
        #if defined(SK_OS_windows)
            // Windows: Use the QueryPerformanceFrequency or other methods
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            return 1;
        #elif defined(SK_OS_macos) || defined(SK_OS_ios)
            uint64_t cpuSpeed;
            size_t size = sizeof(cpuSpeed);
            if (sysctlbyname("hw.cpufrequency", &cpuSpeed, &size, nullptr, 0) == 0) {
                return stoi(std::to_string(cpuSpeed / 1000000));  // Convert to MHz
            }
            return -1;
        #elif defined(SK_OS_linux) || defined(SK_OS_android)
            // Linux: Read /proc/cpuinfo or use /sys to get CPU speed
            std::ifstream cpuInfoFile("/proc/cpuinfo");
            std::string line;
            while (std::getline(cpuInfoFile, line)) {
                if (line.find("cpu MHz") != std::string::npos) {
                    std::string::size_type pos = line.find(":");
                    return line.substr(pos + 1);
                }
            }

            // Try to get from /sys on Linux (can also work for Android)
            std::ifstream cpuSpeedFile("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq");
            if (cpuSpeedFile.is_open()) {
                std::string speed;
                std::getline(cpuSpeedFile, speed);
                return speed;  // CPU speed in kHz
            }

            return -1;
        #else
            return -2;
        #endif
    }

    static SK_CPUInfo getCPUTimes() {
        SK_CPUInfo _cpuInfo;
        #if defined(_WIN32) || defined(_WIN64)
            // Windows: Using GetSystemTimes
            FILETIME idleTime, kernelTime, userTime;
            if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
                ULARGE_INTEGER ulIdleTime, ulKernelTime, ulUserTime;
                ulIdleTime.LowPart = idleTime.dwLowDateTime;
                ulIdleTime.HighPart = idleTime.dwHighDateTime;
                ulKernelTime.LowPart = kernelTime.dwLowDateTime;
                ulKernelTime.HighPart = kernelTime.dwHighDateTime;
                ulUserTime.LowPart = userTime.dwLowDateTime;
                ulUserTime.HighPart = userTime.dwHighDateTime;

                _cpuInfo.user = ulUserTime.QuadPart;
                _cpuInfo.sys = ulKernelTime.QuadPart;
                _cpuInfo.idle = ulIdleTime.QuadPart;
            }
            else {
                std::cerr << "Failed to retrieve CPU times on Windows" << std::endl;
            }

          
            LARGE_INTEGER frequency;
            if (QueryPerformanceFrequency(&frequency)) {
                //std::cout << "Ticks per second: " << frequency.QuadPart << std::endl;
            }
            else {
                //std::cerr << "QueryPerformanceFrequency failed." << std::endl;
            }


            _cpuInfo.user = (_cpuInfo.user * 1000000) / frequency.QuadPart;
            _cpuInfo.sys = (_cpuInfo.sys * 1000000) / frequency.QuadPart;
            _cpuInfo.idle = (_cpuInfo.idle * 1000000) / frequency.QuadPart;


        #elif defined(__linux__) || defined(__ANDROID__)
            // Linux/Android: Using /proc/stat
            std::ifstream procStat("/proc/stat");
            std::string line;
            if (std::getline(procStat, line)) {
                std::istringstream ss(line);
                std::string cpu;
                unsigned long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

                ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

                std::cout << "Linux/Android - User: " << user << std::endl;
                std::cout << "Linux/Android - Nice: " << nice << std::endl;
                std::cout << "Linux/Android - System: " << system << std::endl;
                std::cout << "Linux/Android - Idle: " << idle << std::endl;
                std::cout << "Linux/Android - Iowait: " << iowait << std::endl;
                std::cout << "Linux/Android - IRQ: " << irq << std::endl;
                std::cout << "Linux/Android - SoftIRQ: " << softirq << std::endl;
                std::cout << "Linux/Android - Steal: " << steal << std::endl;
                std::cout << "Linux/Android - Guest: " << guest << std::endl;
                std::cout << "Linux/Android - Guest Nice: " << guest_nice << std::endl;
            }
            else {
                std::cerr << "Failed to read /proc/stat on Linux/Android" << std::endl;
            }

        #elif defined(__APPLE__)
            // macOS: Using host_statistics
            struct host_cpu_load_info cpuLoad;
            mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;

            if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuLoad, &count) == KERN_SUCCESS) {
                std::cout << "macOS - User: " << cpuLoad.cpu_ticks[CPU_STATE_USER] << std::endl;
                std::cout << "macOS - System: " << cpuLoad.cpu_ticks[CPU_STATE_SYSTEM] << std::endl;
                std::cout << "macOS - Idle: " << cpuLoad.cpu_ticks[CPU_STATE_IDLE] << std::endl;
            }
            else {
                std::cerr << "Failed to retrieve CPU load info on macOS" << std::endl;
            }

        #else
            std::cerr << "Unsupported OS" << std::endl;
        #endif

        return _cpuInfo;
    }

    static nlohmann::json getCPUInfo() {
        int coreCount = 0;

        #if defined(SK_OS_windows)
            // Windows: Use GetSystemInfo to get the number of CPU cores
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            coreCount = sysInfo.dwNumberOfProcessors;
        #elif defined(SK_OS_macos) || defined(SK_OS_ios)
            // macOS/iOS: Use sysctl to get the number of CPU cores
            int numCPU;
            size_t len = sizeof(numCPU);
            sysctlbyname("hw.ncpu", &numCPU, &len, nullptr, 0);
            coreCount = numCPU;

        #elif defined(SK_OS_linux)
            // Linux/Android: Use sysctl or read /proc/cpuinfo
            coreCount = std::thread::hardware_concurrency();  // Standard C++ way
        #endif

        nlohmann::json cpu {
            {"coreCount", coreCount}, //used by availableParallelism
        };


        //std::vector<CPUInfo> cpus = getCPUInformation();

        SK_CPUInfo cpuTimes;// = getCPUTimes(); //ignoreing for now

        nlohmann::json cores = nlohmann::json();

        for (int i = 0; i < coreCount; i++) {
            cores.push_back({
                {"model", cpuModel},
                {"speed", cpuSpeed},
                {"times", {
                    {"user", cpuTimes.user},
                    {"nice", cpuTimes.nice},
                    {"sys", cpuTimes.sys},
                    {"idle", cpuTimes.idle},
                    {"irq", cpuTimes.irq},
                }}
            });
        }

        cpu["cores"] = cores;



        return cpu;
    }

    static nlohmann::json getMemoryInfo() {
        #if defined(SK_OS_windows)
            MEMORYSTATUSEX memoryStatus;
            memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);

            if (GlobalMemoryStatusEx(&memoryStatus)){
                
            } else {
                return respondError("Unable to get memory status");
            }

            nlohmann::json info {
                {"physical", {
                    {"free", memoryStatus.ullAvailPhys},
                    {"total", memoryStatus.ullTotalPhys},
                    {"used", memoryStatus.ullTotalPhys - memoryStatus.ullAvailPhys}
                }},

                {"page", {
                    {"free", memoryStatus.ullAvailPageFile},
                    {"total", memoryStatus.ullTotalPageFile},
                    {"used", memoryStatus.ullTotalPageFile - memoryStatus.ullAvailPageFile}
                }},

                {"virtual", {
                    {"free", memoryStatus.ullAvailVirtual},
                    {"total", memoryStatus.ullTotalVirtual},
                    {"used", memoryStatus.ullTotalVirtual - memoryStatus.ullAvailVirtual}
                }},

                {"extendedAvailable", (size_t)memoryStatus.ullAvailExtendedVirtual},
                {"usageInPercent",(size_t)memoryStatus.dwMemoryLoad}
            };
        
        #elif defined(SK_OS_macos) || defined(SK_OS_ios)
        
            SK_MemoryInfo memInfo;

            // Get total memory
            int mib[2] = {CTL_HW, HW_MEMSIZE};
            uint64_t totalMemory;
            size_t size = sizeof(totalMemory);
            if (sysctl(mib, 2, &totalMemory, &size, NULL, 0) == 0) {
                memInfo.totalMemory = totalMemory;
            } else {
                std::cerr << "Failed to get total memory" << std::endl;
                memInfo.totalMemory = 0;
            }

            // Get free memory
            mach_msg_type_number_t count = HOST_BASIC_INFO_COUNT;
            host_basic_info_data_t hostInfo;
            kern_return_t ret = host_info(mach_host_self(), HOST_BASIC_INFO, (host_info_t)&hostInfo, &count);

            /*
            if (ret == KERN_SUCCESS) {
                 memInfo.freeMemory = hostInfo.memory_size - hostInfo.memory_used;
                 memInfo.usedMemory = hostInfo.memory_used;
            } else {
                std::cerr << "Failed to get memory statistics" << std::endl;
                 memInfo.freeMemory = 0;
                 memInfo.usedMemory = 0;
            }
             */
        
        
            nlohmann::json info {
                {"physical", {
                    {"free", 0},
                    {"total", 0},
                    {"used", 0}
                }},

                {"page", {
                    {"free", 0},
                    {"total", 0},
                    {"used", 0}
                }},

                {"virtual", {
                    {"free", 0},
                    {"total", 0},
                    {"used", 0}
                }},

                {"extendedAvailable", 0},
                {"usageInPercent", 0}
            };
        #endif
        
        return info;
    }

    static nlohmann::json getNetworkInfo() {
        nlohmann::json json {

        };

        return json;
    }

    static nlohmann::json getMachineTime() {
        #if defined(SK_OS_windows)
            double number = GetTickCount64();
        #elif defined(SK_OS_macos) || defined(SK_OS_ios)
            uint64_t number = 1;
        
            struct timeval boottime;
            size_t size = sizeof(boottime);

            // Get system boot time
            if (sysctlbyname("kern.boottime", &boottime, &size, NULL, 0) != 0) {
                int x = 0;
            } else {
                
                auto boot_ms = boottime.tv_sec * 1000 + boottime.tv_usec / 1000;
                
                // Current time
                auto now = std::chrono::system_clock::now();
                auto now_time = std::chrono::system_clock::to_time_t(now);
                auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
                
                number = now_ms - boot_ms;
            }
        #endif
        
        double result = number / 1000.0;

        // Store the formatted result in a string variable
        std::ostringstream uptime_three_decimals;
        uptime_three_decimals << std::fixed << std::setprecision(3) << result;
        std::string uptime_three_decimals_str = uptime_three_decimals.str();

        nlohmann::json json {
            {"uptime", stof(uptime_three_decimals_str)}
        };

        return json;
    }

    static nlohmann::json getUserInfo() {
        nlohmann::json info = {
            {"uid", -1},
            {"gid", -1},
            {"username", getUsername()},
            {"homedir", SK_Path_Utils::paths["home"]},
            {"shell", "null"},
        };

        return info;
    }


    static SK_String getUsername() {
        #if defined(SK_OS_windows)
            char username[UNLEN + 1];
            DWORD usernameLen = UNLEN + 1;
            if (GetUserNameA(username, &usernameLen)) {
                return std::string(username);
            }
            else {
                return "UnknownUser"; // Fallback
            }

        #elif defined(SK_OS_macos) || defined(SK_OS_ios) || defined(SK_OS_linux) || defined(SK_OS_android)
            const char* username = std::getenv("USER"); // Try environment variable first
            if (username) {
                return std::string(username);
            }
            else {
                struct passwd* pw = getpwuid(getuid());
                if (pw) {
                    return std::string(pw->pw_name);
                }
                else {
                    return "UnknownUser"; // Fallback
                }
            }

        #else
            return "UnknownUser"; // Generic fallback
        #endif
    }
};

END_SK_NAMESPACE
