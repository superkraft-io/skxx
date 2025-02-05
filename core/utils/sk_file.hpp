#pragma once

#include "../sk_common.hpp"

BEGIN_SK_NAMESPACE

struct SK_File_Time {
	uint64_t atime = -1;
	uint64_t mtime = -1;
	uint64_t ctime = -1;

	SK_String atime_str;
	SK_String mtime_str;
	SK_String ctime_str;
};

struct SK_File_Info {
	uint64_t volumeSerialNumber;
	SK_String mode;
	int numberOfLinks;

	uint64_t ino;

	char* atime;
	char* mtime;
	char* ctime;
};

class SK_File : public SK_String {
public:
	SK_String error;
	SK_String mimeType;
	nlohmann::json fileInfo;

	static bool isPathAbsolute(const SK_String& path) {
		return std::filesystem::path(path).is_absolute();
	}

	bool loadFromDisk(const SK_String& path, bool includeFileInfo = false){
		if (!exists(path)) return false;

		FILE* file = fopen(path.replaceAll("\\", "/").c_str(), "rb");
		if (file) {
			fseek(file, 0, SEEK_END);
			long dataSize = ftell(file);
			char* buffer = (char*)malloc(dataSize + 1);
			fseek(file, 0, SEEK_SET);
			fread(buffer, 1, dataSize, file);

			data.resize(dataSize);
			std::memcpy(data.data(), buffer, dataSize);

			free(buffer);

			fclose(file);

			mimeType = SK_Web_Utils::mime.fromFilename(path);

			if (includeFileInfo) fileInfo = getFileInfo(path);

			return true;
		}

		//__debugbreak(); //Could not load file from disk. Maybe the path is wrong?

		return false;
	}

	bool loadFromBDFS(const SK_String& path){
		//Load from binsry data
	}

	bool loadFromVFS(const SK_String& path) {
		//Load from virtual filesystem
	}


	bool saveToDisk(const SK_String& path) {
		//Save to disk
	}

	bool saveToVFS(const SK_String& path) {
		//Save to virtual fileaystem
	}

	static bool unlink(const SK_String& path) {
		if (!exists(path)) return true;
		if (std::filesystem::remove(path)) {
			return true;
		} else {
			return false;
		}
	}

	static bool append(const SK_String& path, const SK_String& data) {
		if (!exists(path)) return false;

		std::ofstream outFile(path.data, std::ios::app);

		if (outFile.is_open()) {
			outFile << data.data;
			return true;
		}
		else {
			return false;
		}
	}

	static nlohmann::json list(const std::string& path) {
		if (!std::filesystem::exists(path)) return false;

		nlohmann::json entries;

		try {
			for (const auto& entry : std::filesystem::directory_iterator(path)) {
				nlohmann::json fileInfo;

				// Determine file type
				fileInfo["type"] = "unknown";
				if (entry.is_directory()) fileInfo["type"] = "dir";
				else if (entry.is_regular_file()) fileInfo["type"] = "file";
				else if (entry.is_symlink()) fileInfo["type"] = "symlink";

				// Get the file size if it's a regular file
				fileInfo["size"] = entry.is_regular_file() ? entry.file_size() : 0;

				// Get the last modified time
				auto ftime = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
					entry.last_write_time() - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
				std::time_t cftime = std::chrono::system_clock::to_time_t(ftime);
				fileInfo["last_modified"] = std::asctime(std::localtime(&cftime));

				// Get the permissions and format them
				std::filesystem::perms permissions = std::filesystem::status(entry).permissions();
				SK_String permStr;

				// Check and append permissions for owner
				permStr += (permissions & std::filesystem::perms::owner_read) != std::filesystem::perms::none ? "r" : "-";
				permStr += (permissions & std::filesystem::perms::owner_write) != std::filesystem::perms::none ? "w" : "-";
				permStr += (permissions & std::filesystem::perms::owner_exec) != std::filesystem::perms::none ? "x" : "-";

				// Check and append permissions for group
				permStr += (permissions & std::filesystem::perms::group_read) != std::filesystem::perms::none ? "r" : "-";
				permStr += (permissions & std::filesystem::perms::group_write) != std::filesystem::perms::none ? "w" : "-";
				permStr += (permissions & std::filesystem::perms::group_exec) != std::filesystem::perms::none ? "x" : "-";

				// Check and append permissions for others
				permStr += (permissions & std::filesystem::perms::others_read) != std::filesystem::perms::none ? "r" : "-";
				permStr += (permissions & std::filesystem::perms::others_write) != std::filesystem::perms::none ? "w" : "-";
				permStr += (permissions & std::filesystem::perms::others_exec) != std::filesystem::perms::none ? "x" : "-";
				fileInfo["permissions"] = permStr;

				// Store the entry in the vector
				fileInfo["name"] = entry.path().filename().string();
				entries.push_back(fileInfo);
			}

			//std::sort(entries.begin(), entries.end(), [](const SK_FileEntry& a, const SK_FileEntry& b) {
			//	return a.name.toLowerCase() < b.name.toLowerCase();  // Compare in lowercase
			//});
		}
		catch (const std::filesystem::filesystem_error& e) {
			std::cerr << "Error: " << e.what() << '\n';
		}

		return entries;
	}

	/**********************/
	#if defined(SK_OS_windows)
		static std::chrono::system_clock::time_point fileTimeToChrono(const FILETIME& ft) {
			ULARGE_INTEGER ull;
			ull.LowPart = ft.dwLowDateTime;
			ull.HighPart = ft.dwHighDateTime;
			const auto duration = std::chrono::nanoseconds(ull.QuadPart - 116444736000000000LL); // Windows FILETIME offset

			// Correct way to convert nanoseconds to time_point
			return std::chrono::system_clock::time_point(std::chrono::duration_cast<std::chrono::system_clock::duration>(duration));
		}

	#elif defined(SK_OS_macos) || defined(SK_OS_ios)
        std::chrono::system_clock::time_point timespecToChrono(const struct timespec& ts) {
            // Convert seconds and nanoseconds to a duration
            auto duration = std::chrono::seconds(ts.tv_sec) + std::chrono::nanoseconds(ts.tv_nsec);

            // Construct a time_point by adding the duration to the system clock's epoch
            return std::chrono::system_clock::time_point(
                std::chrono::duration_cast<std::chrono::system_clock::duration>(duration)
            );
        }
	#endif

	static uint64_t timePointToUint64(const std::chrono::system_clock::time_point& timePoint) {
		auto durationSinceEpoch = timePoint.time_since_epoch();
		return std::chrono::duration_cast<std::chrono::nanoseconds>(durationSinceEpoch).count();
	}


	static void getFileTimestamps(const SK_String& path, SK_File_Time* _fileTime) {
		if (!exists(path)) return;

		SK_File_Time fileTime;

		#if defined(SK_OS_windows)
			FILETIME creationTime, lastAccessTime, lastWriteTime;

			WIN32_FILE_ATTRIBUTE_DATA fileData;
			if (GetFileAttributesExA(path.data.c_str(), GetFileExInfoStandard, &fileData)) {

				fileTime.ctime_str = SK_DateTime::formatFileTime(fileData.ftCreationTime);
				fileTime.atime_str = SK_DateTime::formatFileTime(fileData.ftLastAccessTime);
				fileTime.mtime_str = SK_DateTime::formatFileTime(fileData.ftLastWriteTime);

				fileTime.ctime = timePointToUint64(fileTimeToChrono(fileData.ftCreationTime));
				fileTime.atime = timePointToUint64(fileTimeToChrono(fileData.ftLastAccessTime));
				fileTime.mtime = timePointToUint64(fileTimeToChrono(fileData.ftLastWriteTime));
			}
			else {
				__debugbreak();
			}

		#elif defined(SK_OS_macos) || defined(SK_OS_ios)
			struct stat fileStat;
			if (stat(path.data.c_str(), &fileStat) == 0) {
                auto timespecToChrono = [](const struct timespec& ts) {
                    return std::chrono::system_clock::time_point{std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds(ts.tv_sec) + std::chrono::nanoseconds(ts.tv_nsec))};
                };

				auto creation = timespecToChrono(fileStat.st_ctimespec);
				auto access = timespecToChrono(fileStat.st_atimespec);
				auto modification = timespecToChrono(fileStat.st_mtimespec);

				std::cout << "Creation Time: " << std::chrono::system_clock::to_time_t(creation) << std::endl;
				std::cout << "Last Access Time: " << std::chrono::system_clock::to_time_t(access) << std::endl;
				std::cout << "Last Modification Time: " << std::chrono::system_clock::to_time_t(modification) << std::endl;
			}
			else {
				std::cerr << "Error: Cannot get file stats." << std::endl;
			}
		#endif

		_fileTime->ctime_str = fileTime.ctime_str;
		_fileTime->atime_str = fileTime.atime_str;
		_fileTime->mtime_str = fileTime.mtime_str;

		_fileTime->atime = fileTime.atime;
		_fileTime->ctime = fileTime.ctime;
		_fileTime->mtime = fileTime.mtime;
	}

    static bool exists(const SK_String& path){
		try {
			return std::filesystem::exists(std::filesystem::path(path));
		}
		catch (const std::filesystem::filesystem_error& e) {
			debug_break();
		}

		return false;
    }

    static bool isDirectory(const SK_String& path) {
		if (!exists(path)) return false;

        return std::filesystem::is_directory(std::filesystem::path(path));
    }

    static nlohmann::json getFileInfo(const SK_String& path) {
		nlohmann::json statInfo{ {"error", "ENOENT"} };

		if (!exists(path)) return statInfo;


		SK_File_Info fileInfo;

		SK_File_Time fileTime;
		getFileTimestamps(path, &fileTime);


		if (isDirectory(path)) {
			uint64_t size = static_cast<uint64_t>(std::filesystem::file_size(std::filesystem::path(path)));
			statInfo = nlohmann::json {
				{"type"			, "dir"},
				{"dev"			, "" },
				{"mode"			, -1 },
				{"nlink"		, 0  },
				{"uid"			, 0  },
				{"gid"			, 0  },
				{"rdev"			, 0  },
				{"blksize"		, -1 },
				{"ino"			, 0  },
				{"size"			, size },
				{"blocks"		, -1 },
				{"atimeMs"		, fileTime.atime },
				{"mtimeMs"		, fileTime.mtime },
				{"ctimeMs"		, fileTime.ctime },
				{"birthtimeMs"	, fileTime.ctime },
				{"atime"		, fileTime.atime_str },
				{"mtime"		, fileTime.mtime_str },
				{"ctime"		, fileTime.ctime_str },
				{"birthtime"	, fileTime.ctime_str }
			};
		} else {
#			if defined(SK_OS_windows)
				HANDLE hFile = CreateFile(path.data.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFile == INVALID_HANDLE_VALUE) {
					//return error "ENOENT"
					return statInfo;
				}

				BY_HANDLE_FILE_INFORMATION _fileInfo;
				if (!GetFileInformationByHandle(hFile, &_fileInfo)) {
					//return error "ENOENT"
					CloseHandle(hFile);
					return statInfo;
				}

				CloseHandle(hFile);

				fileInfo.volumeSerialNumber = (uint64_t)_fileInfo.dwVolumeSerialNumber;
				fileInfo.numberOfLinks = (uint64_t)_fileInfo.nNumberOfLinks;
				fileInfo.ino = ((uint64_t)_fileInfo.nFileIndexHigh << 32) | _fileInfo.nFileIndexLow;

			#elif defined(SK_OS_macos) || defined(SK_OS_ios)

				struct stat fileStat;

				// Get file statistics
				if (stat(path.data.c_str(), &fileStat) != 0) {
					std::cerr << "Failed to get file stats for " << path.data.c_str() << std::endl;
					return statInfo;
				}

				// Print file size
				//std::cout << "File size: " << fileStat.st_size << " bytes" << std::endl;

				// Print file permissions
				char* permissions;
				/*permissions << ((fileStat.st_mode & S_IRUSR) ? "r" : "-")
							  << ((fileStat.st_mode & S_IWUSR) ? "w" : "-")
							  << ((fileStat.st_mode & S_IXUSR) ? "x" : "-")
							  << ((fileStat.st_mode & S_IRGRP) ? "r" : "-")
							  << ((fileStat.st_mode & S_IWGRP) ? "w" : "-")
							  << ((fileStat.st_mode & S_IXGRP) ? "x" : "-")
							  << ((fileStat.st_mode & S_IROTH) ? "r" : "-")
							  << ((fileStat.st_mode & S_IWOTH) ? "w" : "-")
							  << ((fileStat.st_mode & S_IXOTH) ? "x" : "-")
							  << std::endl;
				*/

				fileInfo.atime = std::ctime(&fileStat.st_atime);
				fileInfo.mtime = std::ctime(&fileStat.st_mtime);
				fileInfo.ctime = std::ctime(&fileStat.st_ctime);
			#endif



			statInfo = nlohmann::json{
				{"type"			, "file"},
				{"dev"			, fileInfo.volumeSerialNumber },
				{"mode"			, -1 },
				{"nlink"		, fileInfo.numberOfLinks },
				{"uid"			, 0  },
				{"gid"			, 0  },
				{"rdev"			, 0  },
				{"blksize"		, -1 },
				{"ino"			, fileInfo.ino },
				{"size"			, std::filesystem::file_size(std::filesystem::path(path)) },
				{"blocks"		, -1 },
				{"atimeMs"		, fileTime.atime },
				{"mtimeMs"		, fileTime.mtime },
				{"ctimeMs"		, fileTime.ctime },
				{"birthtimeMs"	, fileTime.ctime },
				{"atime"		, fileTime.atime_str },
				{"mtime"		, fileTime.mtime_str },
				{"ctime"		, fileTime.ctime_str },
				{"birthtime"	, fileTime.ctime_str }
			};
		}

		return statInfo;
    }
};


END_SK_NAMESPACE
