#pragma once

#include "string"
#include "json.hpp"
#include <iostream>
#include <sstream>
#include <regex>
#include <algorithm>
#include "../../sk_var.hpp"


#ifdef __OBJC__
    #import <Foundation/Foundation.h>
#else
    // Forward declarations for C++ contexts
    #if defined(__APPLE__)
        typedef struct objc_object NSString;
        typedef struct objc_object NSURL;
        typedef struct objc_object NSData;
    #endif
#endif

BEGIN_SK_NAMESPACE

class SK_String {
public:
    std::string data;
    
    // Constructors
    SK_String();
    SK_String(const char *s);
    SK_String(const std::string& s);
    SK_String(const SK_String& s);
    SK_String(const nlohmann::json& json);
    SK_String(const std::vector<char>& chars);
    
#ifdef __OBJC__
    SK_String(NSString* nsStr);
    
    operator NSString* () const;
    operator NSURL* () const;
    operator NSData* () const;
#endif

    // Conversion operators
    
    operator std::string() const;
    operator const std::filesystem::path () const;
    explicit operator const char* () const;

    // Methods
    char charAt(size_t index) const;
    int charCodeAt(size_t index) const;
    bool includes(const std::string& substr) const;
    bool startsWith(const std::string& substr) const;
    bool endsWith(const std::string& substr) const;
    int indexOf(const std::string& searchValue) const;
    int lastIndexOf(const std::string& searchValue) const;
    int length() const;
    static SK_String concat(const SK_String& str1, const SK_String& str2);
    SK_String padStart(size_t targetLength, char padChar = ' ') const;
    SK_String padEnd(size_t targetLength, char padChar = ' ') const;
    SK_String repeat(size_t count) const;
    SK_String slice(size_t start, size_t end) const;
    SK_String substring(size_t start, size_t end) const;
    SK_String toLowerCase() const;
    SK_String toUpperCase() const;
    SK_String trim() const;
    std::vector<std::string> split(const SK_String& delimiter) const;
    static SK_String join(const std::vector<SK_String>& arr, const std::string& separator = "");
    SK_String replace(const std::string& pattern, const std::string& replacement) const;
    SK_String replaceAll(const std::string& pattern, const std::string& replacement) const;
    std::smatch match(const std::string& pattern) const;
    const char* c_str();
    const std::string& raw() const;

    // Operator overloads
    SK_String operator+(const char* other) const;
    SK_String operator+(const SK_String& other) const;
    SK_String operator+(const std::string& other) const;
    friend SK_String operator+(const std::string& lhs, const SK_String& rhs);
    SK_String& operator+=(const SK_String& other);
    SK_String& operator+=(const std::string& other);
    SK_String& operator+=(const char* other);
    bool operator==(const SK_String& other) const;
    bool operator==(const std::string& other) const;
    bool operator==(const char* other) const;
    friend bool operator==(const std::string& lhs, const SK_String& rhs);
    friend bool operator==(const char* lhs, const SK_String& rhs);
    bool operator!=(const SK_String& other) const;
    bool operator!=(const std::string& other) const;
    bool operator!=(const char* other) const;
    friend bool operator!=(const std::string& lhs, const SK_String& rhs);
    friend bool operator!=(const char* lhs, const SK_String& rhs);

    // Stream operator
    friend std::ostream& operator<<(std::ostream& os, const SK_String& skStr);

    // Base64 methods
    bool is_base64_char(unsigned char c);
    std::string toBase64();
    SK_String fromBase64();
};

END_SK_NAMESPACE
