#pragma once

#include "../sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Web_MIME_utils {
public:
    static std::string fromFileExt(const std::string& ext, const std::string& defaultMimeType = "text/html") {
        if (auto iterator{ mimeTypes.find(ext) }; iterator != mimeTypes.end()) {
            return iterator->second;
        }
        else {
            return defaultMimeType;
        }
    };

    static std::string fromFilename(const std::string& filename, const std::string& defaultMimeType = "text/html") {
        std::string ext = defaultMimeType;

        auto step1 = std::filesystem::path(filename);
        auto step2 = step1.extension();
        auto step3 = step2.string();

        if (step3.empty() == true) return defaultMimeType;
        else ext = step3.substr(1);

        std::string result = fromFileExt(ext, defaultMimeType);
        return result;
    }

    static inline std::unordered_map<std::string, std::string> mimeTypes{
        {"*","application/octet-stream"},
        {"aac", "audio/aac"},
        {"aif", "audio/aiff"},
        {"aiff", "audio/aiff"},
        {"avif", "image/avif"},
        {"bmp", "image/bmp"},
        {"css", "text/css"},
        {"csv", "text/csv"},
        {"flac", "audio/flac"},
        {"gif", "image/gif"},
        {"htm", "text/html"},
        {"html", "text/html"},
        {"ico", "image/vnd.microsoft.icon"},
        {"jpeg", "image/jpeg"},
        {"jpg", "image/jpeg"},
        {"js", "text/javascript"},
        {"json", "application/json"},
        {"md", "text/markdown"},
        {"mid", "audio/midi"},
        {"midi", "audio/midi"},
        {"mjs", "text/javascript"},
        {"mp3", "audio/mpeg"},
        {"mp4", "video/mp4"},
        {"mpeg", "video/mpeg"},
        {"ogg", "audio/ogg"},
        {"otf", "font/otf"},
        {"pdf", "application/pdf"},
        {"png", "image/png"},
        {"rtf", "application/rtf"},
        {"svg", "image/svg+xml"},
        {"svgz", "image/svg+xml"},
        {"tif", "image/tiff"},
        {"tiff", "image/tiff"},
        {"ttf", "font/ttf"},
        {"txt", "text/plain"},
        {"wasm", "application/wasm"},
        {"wav", "audio/wav"},
        {"weba", "audio/webm"},
        {"webm", "video/webm"},
        {"webp", "image/webp"},
        {"woff", "font/woff"},
        {"woff2", "font/woff2"},
        {"xml", "application/xml"},
        {"zip", "application/zip"},
    };
};

static class SK_Web_Utils {
public:
    static SK_Web_MIME_utils mime;
};

END_SK_NAMESPACE
