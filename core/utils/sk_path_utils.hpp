#pragma once


#include "../sk_common.hpp"
#include "sk_string.h"

BEGIN_SK_NAMESPACE

static class SK_Path_Utils {
public:
	std::map<std::string, std::string> paths;

    std::string getAbsoluteFilePath() {
        std::filesystem::path filePath(__FILE__);

        try {
            return std::filesystem::canonical(filePath).string();
        } catch (const std::exception& e) {
            std::cerr << "Error resolving file path: " << e.what() << "\n";
            return filePath.string(); // Fallback to original
        }
    }
    
	std::string pathBackwardsUntilNeighbour(const std::string& neighbourName){
		std::filesystem::path currentPath(getAbsoluteFilePath());

		bool stop = false;
		while (!stop) {
			currentPath = currentPath.parent_path();

			if (currentPath == currentPath.root_path()) {
				return "";
			}

            try {
                for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
                    SK_String _path = SK_String(entry.path().filename().string());
                    
                    if (entry.is_directory() && _path.toLowerCase() == SK_String(neighbourName).toLowerCase()) {
                        stop = true;
                        break;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error accessing directory: " << e.what() << ". If you're on MacOS using Xcode, it may be due to sandboxin in your Xcode project settings. Remove sandboxing.\n";
                int x = 0;
            }
		}

		return currentPath.string();
	}


	void add(const SK_String& id, const SK_String& path) {
		paths[id] = path.replaceAll("\\", "/");
	}


	void init() {
        #if defined(SK_BUNDLE_MODE_NONE)
            //if we're not running in bundled mode, projectRoot will be set to the root folder of your project.
            SK_String projectRoot = SK_String(SK_Path_Utils::pathBackwardsUntilNeighbour("skxx")).replaceAll("\\", "/");
            SK_String skxx = projectRoot + "/skxx";
            add("skxx", skxx);
            add("global_js_core", skxx + "/web/global_js_core");
            add("soft_backend", skxx + "/web/soft_backend");
            add("module_system", skxx + "/module_system/web");
            add("project", projectRoot + "/project");
            add("config", paths["project"] + "/config.json");
        #else
            //but if we are running in bundled mode, projectRoot will be set to "/"".
            SK_String projectRoot = "";
            SK_String skxx = "";
            add("skxx", skxx);
            add("global_js_core", skxx + "sk:webcore/global_js_core");
            add("soft_backend", skxx + "sk:webcore/soft_backend");
            add("module_system", skxx + "sk:modsys");
            add("project", projectRoot + "/");
            add("config", paths["project"] + "config.json");
        #endif

		

		
		
		//The paths below will be OS specific
		add("home", getHomeFolder());
		add("temp", getTempFolder());
	}

	SK_String getTempFolder() {
		#if defined(SK_OS_windows)
			char tempPath[MAX_PATH];
			if (GetTempPathA(MAX_PATH, tempPath)) {
				return std::string(tempPath);
			}
			else {
				return "C:\\Temp"; // Fallback
			}

		#elif defined(SK_OS_macos) || defined(SK_OS_ios)
			const char* tempPath = std::getenv("TMPDIR");
			if (tempPath) {
				return std::string(tempPath);
			}
			else {
				return "/tmp"; // Fallback
			}

		#elif defined(SK_OS_android)
			const char* tempPath = std::getenv("TMPDIR");
			if (tempPath) {
				return std::string(tempPath);
			}
			else {
				return "/data/local/tmp"; // Common temp directory for Android
			}

		#elif defined(SK_OS_linux)
			const char* tempPath = std::getenv("TMPDIR");
			if (tempPath) {
				return std::string(tempPath);
			}
			else {
				return "/tmp"; // Fallback for Linux
			}

		#else
			return "/tmp"; // Generic fallback
		#endif
	}

	SK_String getHomeFolder() {
		#if defined(SK_OS_windows)
			char homePath[MAX_PATH];
			if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, homePath))) {
				return std::string(homePath);
			}
			else {
				return "C:\\Users\\Default"; // Fallback
			}

		#elif defined(SK_OS_macos) || defined(SK_OS_ios)
			const char* homePath = std::getenv("HOME");
			if (homePath) {
				return std::string(homePath);
			}
			else {
				struct passwd* pw = getpwuid(getuid());
				if (pw) {
					return std::string(pw->pw_dir);
				}
				else {
					return "/Users/Default"; // Fallback
				}
			}

		#elif defined(SK_OS_android)
			const char* homePath = std::getenv("HOME");
			if (homePath) {
				return std::string(homePath);
			}
			else {
				return "/data/data"; // Common root directory for Android apps
			}

		#elif defined(SK_OS_linux)
			const char* homePath = std::getenv("HOME");
			if (homePath) {
				return std::string(homePath);
			}
			else {
				struct passwd* pw = getpwuid(getuid());
				if (pw) {
					return std::string(pw->pw_dir);
				}
				else {
					return "/home/default"; // Fallback
				}
			}

		#else
			return "/"; // Generic fallback
		#endif
	}

	SK_String GetOSFolder(const SK_String& __id, int flags = 0) {

		SK_String _id = __id.toUpperCase();

		#if defined(SK_OS_windows)
			int id = -1;

			std::map<std::string, int> ids {
				{ "DESKTOP", CSIDL_DESKTOP},
				{ "INTERNET", CSIDL_INTERNET },
				{ "PROGRAMS", CSIDL_PROGRAMS },
				{ "CONTROLS", CSIDL_CONTROLS },
				{ "PRINTERS", CSIDL_PRINTERS },
				{ "PERSONAL", CSIDL_PERSONAL },
				{ "FAVORITES", CSIDL_FAVORITES },
				{ "STARTUP", CSIDL_STARTUP },
				{ "RECENT", CSIDL_RECENT },
				{ "SENDTO", CSIDL_SENDTO },
				{ "BITBUCKET", CSIDL_BITBUCKET },
				{ "STARTMENU", CSIDL_STARTMENU },
				{ "MYDOCUMENTS", CSIDL_MYDOCUMENTS },
				{ "MYMUSIC", CSIDL_MYMUSIC },
				{ "MYVIDEO", CSIDL_MYVIDEO },
				{ "DESKTOPDIRECTORY", CSIDL_DESKTOPDIRECTORY },
				{ "DRIVES", CSIDL_DRIVES },
				{ "NETWORK", CSIDL_NETWORK },
				{ "NETHOOD", CSIDL_NETHOOD },
				{ "FONTS", CSIDL_FONTS },
				{ "TEMPLATES", CSIDL_TEMPLATES },
				{ "COMMON_STARTMENU", CSIDL_COMMON_STARTMENU },
				{ "COMMON_PROGRAMS", CSIDL_COMMON_PROGRAMS },
				{ "COMMON_STARTUP", CSIDL_COMMON_STARTUP },
				{ "COMMON_DESKTOPDIRECTORY", CSIDL_COMMON_DESKTOPDIRECTORY },
				{ "APPDATA", CSIDL_APPDATA },
				{ "PRINTHOOD", CSIDL_PRINTHOOD },

				{ "ALTSTARTUP", CSIDL_ALTSTARTUP },
				{ "COMMON_ALTSTARTUP", CSIDL_COMMON_ALTSTARTUP },
				{ "COMMON_FAVORITES", CSIDL_COMMON_FAVORITES },

				{ "INTERNET_CACHE", CSIDL_INTERNET_CACHE },
				{ "COOKIES", CSIDL_COOKIES },
				{ "HISTORY", CSIDL_HISTORY },
				{ "COMMON_APPDATA", CSIDL_COMMON_APPDATA },
				{ "WINDOWS", CSIDL_WINDOWS },
				{ "SYSTEM", CSIDL_SYSTEM },
				{ "PROGRAM_FILES", CSIDL_PROGRAM_FILES },
				{ "MYPICTURES", CSIDL_MYPICTURES },


				{ "PROFILE", CSIDL_PROFILE },
				{ "SYSTEMX86", CSIDL_SYSTEMX86 },
				{ "PROGRAM_FILESX86", CSIDL_PROGRAM_FILESX86 },

				{ "PROGRAM_FILES_COMMON", CSIDL_PROGRAM_FILES_COMMON },

				{ "PROGRAM_FILES_COMMONX86", CSIDL_PROGRAM_FILES_COMMONX86 },
				{ "COMMON_TEMPLATES", CSIDL_COMMON_TEMPLATES },

				{ "COMMON_DOCUMENTS", CSIDL_COMMON_DOCUMENTS },
				{ "COMMON_ADMINTOOLS", CSIDL_COMMON_ADMINTOOLS },
				{ "ADMINTOOLS", CSIDL_ADMINTOOLS },

				{ "CONNECTIONS", CSIDL_CONNECTIONS },
				{ "COMMON_MUSIC", CSIDL_COMMON_MUSIC },
				{ "COMMON_PICTURES", CSIDL_COMMON_PICTURES },
				{ "COMMON_VIDEO", CSIDL_COMMON_VIDEO },
				{ "RESOURCES", CSIDL_RESOURCES },

				{ "RESOURCES_LOCALIZED", CSIDL_RESOURCES_LOCALIZED },

				{ "COMMON_OEM_LINKS", CSIDL_COMMON_OEM_LINKS },
				{ "CDBURN_AREA", CSIDL_CDBURN_AREA }
			};

			if (ids.find(_id) != ids.end()) {
				id = ids[_id];
			}

			wchar_t _path[1024];

			SHGetFolderPathW(NULL, id, NULL, flags, _path);

			return SK_String(_path);
		#elif defined(SK_OS_apple)
			//for apple
            return "";
		#endif
	}

};


END_SK_NAMESPACE
