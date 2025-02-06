#pragma once


#include "sk_string_apple.h"
#import <Foundation/Foundation.h>


BEGIN_SK_NAMESPACE



// Constructors
SK_String::SK_String() : data("") {}
SK_String::SK_String(const char *s) : data(s) {}
SK_String::SK_String(const std::string& s) : data(s) {}
SK_String::SK_String(const SK_String& s) : data(s.data) {}
SK_String::SK_String(const nlohmann::json& json) : data(json.get<std::string>()) {}
SK_String::SK_String(const std::vector<char>& chars) : data(chars.data(), chars.size()) {}


#ifdef __OBJC__
SK_String::SK_String(NSString* nsStr) {
    if (nsStr) {
        data = std::string([nsStr UTF8String]);
    } else {
        data.clear();
    }
}

// Conversion operators
SK_String::operator NSString* () const {
    return [NSString stringWithUTF8String:data.c_str()];
}

SK_String::operator NSURL* () const {
    return [NSURL URLWithString:[NSString stringWithUTF8String:data.c_str()]];
}

SK_String::operator NSData* () const {
    return [(NSString*)*this dataUsingEncoding:NSUTF8StringEncoding];
}

#endif


SK_String::operator std::string() const {
    return data;
}

SK_String::operator const std::filesystem::path () const {
    return data;
}

SK_String::operator const char* () const {
    return data.c_str();
}

// Methods
char SK_String::charAt(size_t index) const {
    if (index >= data.length()) throw std::out_of_range("Index out of range");
    return data.at(index);
}

int SK_String::charCodeAt(size_t index) const {
    if (index >= data.length()) throw std::out_of_range("Index out of range");
    return static_cast<int>(data.at(index));
}

bool SK_String::includes(const std::string& substr) const {
    return data.find(substr) != std::string::npos;
}

bool SK_String::startsWith(const std::string& substr) const {
    return data.rfind(substr, 0) == 0;
}

bool SK_String::endsWith(const std::string& substr) const {
    if (substr.size() > data.size()) return false;
    return std::equal(substr.rbegin(), substr.rend(), data.rbegin());
}

int SK_String::indexOf(const std::string& searchValue) const {
    return data.find(searchValue);
}

int SK_String::lastIndexOf(const std::string& searchValue) const {
    return data.rfind(searchValue);
}

int SK_String::length() const {
    return data.length();
}

SK_String SK_String::concat(const SK_String& str1, const SK_String& str2) {
    return SK_String(str1.data + str2.data);
}

SK_String SK_String::padStart(size_t targetLength, char padChar) const {
    if (data.length() >= targetLength) return *this;
    return SK_String(std::string(targetLength - data.length(), padChar) + data);
}

SK_String SK_String::padEnd(size_t targetLength, char padChar) const {
    if (data.length() >= targetLength) return *this;
    return SK_String(data + std::string(targetLength - data.length(), padChar));
}

SK_String SK_String::repeat(size_t count) const {
    std::string result;
    for (size_t i = 0; i < count; ++i) {
        result += data;
    }
    return SK_String(result);
}

SK_String SK_String::slice(size_t start, size_t end) const {
    return SK_String(data.substr(start, end - start));
}

SK_String SK_String::substring(size_t start, size_t end) const {
    return SK_String(data.substr(start, end - start));
}

SK_String SK_String::toLowerCase() const {
    std::string result = data;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return SK_String(result);
}

SK_String SK_String::toUpperCase() const {
    std::string result = data;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return SK_String(result);
}

SK_String SK_String::trim() const {
    size_t first = data.find_first_not_of(" \t\n\r");
    size_t last = data.find_last_not_of(" \t\n\r");
    if (first == std::string::npos || last == std::string::npos) return SK_String("");
    return SK_String(data.substr(first, (last - first + 1)));
}

std::vector<std::string> SK_String::split(const SK_String& delimiter) const {
    std::vector<std::string> result;
    std::string token;
    std::stringstream ss(data);
    char _delimiter = delimiter.data[0];
    while (std::getline(ss, token, _delimiter)) {
        result.push_back(SK_String(token));
    }
    return result;
}

SK_String SK_String::join(const std::vector<SK_String>& arr, const std::string& separator) {
    std::string result;
    for (size_t i = 0; i < arr.size(); ++i) {
        result += arr[i].data;
        if (i < arr.size() - 1) {
            result += separator;
        }
    }
    return SK_String(result);
}

SK_String SK_String::replace(const std::string& pattern, const std::string& replacement) const {
    std::string result = data;
    size_t pos = 0;
    while ((pos = result.find(pattern, pos)) != std::string::npos) {
        result.replace(pos, pattern.length(), replacement);
        pos += replacement.length();
    }
    return SK_String(result);
}

SK_String SK_String::replaceAll(const std::string& pattern, const std::string& replacement) const {
    return replace(pattern, replacement);
}

std::smatch SK_String::match(const std::string& pattern) const {
    std::smatch matches;
    std::regex_search(data, matches, std::regex(pattern));
    return matches;
}

const char* SK_String::c_str() {
    return data.c_str();
}

const std::string& SK_String::raw() const {
    return data;
}

bool SK_String::is_base64_char(unsigned char c) {
    return (c >= 65 && c <= 90) || (c >= 97 && c <= 122) || (c >= 48 && c <= 57) || (c == 43 || c == 47);
}

std::string SK_String::toBase64() {
    std::string base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string encoded;
    unsigned char charArray3[3];
    unsigned char charArray4[4];
    int i = 0;
    int j = 0;
    size_t len = data.length();
    while (len--) {
        charArray3[i++] = data[j++];
        if (i == 3) {
            charArray4[0] = (charArray3[0] & 0xfc) >> 2;
            charArray4[1] = ((charArray3[0] & 0x03) << 4) | ((charArray3[1] & 0xf0) >> 4);
            charArray4[2] = ((charArray3[1] & 0x0f) << 2) | ((charArray3[2] & 0xc0) >> 6);
            charArray4[3] = charArray3[2] & 0x3f;
            for (i = 0; i < 4; i++) {
                encoded += base64Chars[charArray4[i]];
            }
            i = 0;
        }
    }
    if (i) {
        for (j = i; j < 3; j++) {
            charArray3[j] = '\0';
        }
        charArray4[0] = (charArray3[0] & 0xfc) >> 2;
        charArray4[1] = ((charArray3[0] & 0x03) << 4) | ((charArray3[1] & 0xf0) >> 4);
        charArray4[2] = ((charArray3[1] & 0x0f) << 2) | ((charArray3[2] & 0xc0) >> 6);
        for (j = 0; j < i + 1; j++) {
            encoded += base64Chars[charArray4[j]];
        }
        while (encoded.length() % 4) {
            encoded += "=";
        }
    }
    return encoded;
}

SK_String SK_String::fromBase64() {
    std::string decoded;
    std::string base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int inStrLen = data.size();
    int i = 0;
    int j = 0;
    unsigned char charArray4[4], charArray3[3];
    while (inStrLen-- && (data[j] != '=') && is_base64_char(data[j])) {
        charArray4[i++] = data[j++];
        if (i == 4) {
            for (i = 0; i < 4; i++) {
                charArray4[i] = base64Chars.find(charArray4[i]);
            }
            charArray3[0] = (charArray4[0] << 2) | (charArray4[1] >> 4);
            charArray3[1] = ((charArray4[1] & 0x0f) << 4) | (charArray4[2] >> 2);
            charArray3[2] = ((charArray4[2] & 0x03) << 6) | charArray4[3];
            for (i = 0; i < 3; i++) {
                decoded += charArray3[i];
            }
            i = 0;
        }
    }
    return SK_String(decoded);
}

END_SK_NAMESPACE
