#pragma once


#include "../sk_common.hxx"


BEGIN_SK_NAMESPACE

static class SK_Path_Utils {
public:
	static inline std::map<std::string, std::string> paths;

	static std::string pathBackwardsUntilNeighbour(const std::string& neighbourName){
		std::filesystem::path currentPath(__FILE__);

		bool stop = false;
		while (!stop) {
			currentPath = currentPath.parent_path();

			if (currentPath == currentPath.root_path()) {
				return "";
			}

			for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
				if (entry.is_directory() && SK_String(entry.path().filename().string()).toLowerCase() == SK_String(neighbourName).toLowerCase()) {
					stop = true;
					break;
				}
			}
		}

		return currentPath.string();
	}


	static void add(const SK_String& id, const SK_String& path) {
		paths[id] = path.replaceAll("\\", "/");
	}


	static void init() {
		#ifdef SK_MODE_DEBUG
			//In DEBUG mode the projectRoot will be set to the root folder of your project.
			SK_String projectRoot = SK_String(SK_Path_Utils::pathBackwardsUntilNeighbour("skxx")).replaceAll("\\", "/");
		#else
			//In RELEASE mode the projectRoot will be set to the virtual file system called Binary Data File Systtem
			//When running in RELEASE mode, a pre-script will bundle all the files in "superkraft" and "SK_Project" and SK will access those files from memory instead.
			SK_String projectRoot = "sk_bdfs:";
		#endif

		SK_String skxx = projectRoot + "/skxx";

		add("skxx", skxx);
		add("global_js_core", skxx + "/web/global_js_core");
		add("module_system", skxx + "/module_system/web");
		add("soft_backend", skxx + "/web/soft_backend");
		add("project", projectRoot + "/project");
		add("config", paths["project"] + "/config.json");
		
		//The paths below will be OS specific
		add("home", getHomeFolder());
		add("temp", getTempFolder());
	}

	static SK_String getTempFolder() {
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

	static SK_String getHomeFolder() {
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
};


END_SK_NAMESPACE