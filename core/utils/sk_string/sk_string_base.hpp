#pragma once

#include "../../sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_String_Base {
private:

public:
    std::string data;

    // Constructor
    SK_String_Base() : data("") {}
    SK_String_Base(const char *s) : data(s) {}
    SK_String_Base(const std::string& s) : data(s) {}
    SK_String_Base(const SK_String_Base& s) : data(s) {}
    SK_String_Base(const nlohmann::json& json) : data(json.get<std::string>()) {}
    SK_String_Base(const std::vector<char>& chars) : data(chars.data(), chars.size()) {}

    /*
    SK_String_Base(const std::int8_t n) : data(std::to_string(n)) {}
    SK_String_Base(const std::uint8_t n) : data(std::to_string(n)) {}
    SK_String_Base(const std::int16_t n) : data(std::to_string(n)) {}
    SK_String_Base(const std::uint16_t n) : data(std::to_string(n)) {}
    SK_String_Base(const std::int32_t n) : data(std::to_string(n)) {}
    SK_String_Base(const std::uint32_t n) : data(std::to_string(n)) {}
    SK_String_Base(const std::int64_t n) : data(std::to_string(n)) {}
    SK_String_Base(const std::uint64_t n) : data(std::to_string(n)) {}
    SK_String_Base(const long n) : data(std::to_string(n)) {}
    SK_String_Base(const unsigned long n) : data(std::to_string(n)) {}
    SK_String_Base(const long long n) : data(std::to_string(n)) {}
    SK_String_Base(const unsigned long long n) : data(std::to_string(n)) {}

    SK_String_Base(const float& n, const int& precision) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << n;
        data = oss.str();
    }

    SK_String_Base(const double& n, const int& precision) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << n;
        data = oss.str();
    }

    SK_String_Base(const long double& n, const int& precision) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << n;
        data = oss.str();
    }
    */




    // Method to get the character at a specified index (charAt)
    char charAt(size_t index) const {
        if (index >= data.length()) throw std::out_of_range("Index out of range");
        return data.at(index);
    }

    // Method to get the character code at a specified index (charCodeAt)
    int charCodeAt(size_t index) const {
        if (index >= data.length()) throw std::out_of_range("Index out of range");
        return static_cast<int>(data.at(index));
    }

    // Method to check if the string includes a substring (includes)
    bool includes(const std::string& substr) const {
        return data.find(substr) != std::string::npos;
    }

    // Method to check if the string starts with a specified substring (startsWith)
    bool startsWith(const std::string& substr) const {
        return data.rfind(substr, 0) == 0;
    }

    // Method to check if the string ends with a specified substring (endsWith)
    bool endsWith(const std::string& substr) const {
        if (substr.size() > data.size()) return false;
        return std::equal(substr.rbegin(), substr.rend(), data.rbegin());
    }

    // Method to find the first occurrence of a substring (indexOf)
    int indexOf(const std::string& searchValue) const {
        int pos = data.find(searchValue);
        return pos;
    }

    // Method to find the last occurrence of a substring (lastIndexOf)
    int lastIndexOf(const std::string& searchValue) const {
        return data.rfind(searchValue);
    }

    // Method to get the length of the string (length)
    int length() const {
        return data.length();
    }

    // Method to concatenate two strings (concat)
    static SK_String_Base concat(const SK_String_Base& str1, const SK_String_Base& str2) {
        return SK_String_Base(str1.data + str2.data);
    }

    // Method to pad the start of the string with a specified character (padStart)
    SK_String_Base padStart(size_t targetLength, char padChar = ' ') const {
        if (data.length() >= targetLength) return *this;
        return SK_String_Base(std::string(targetLength - data.length(), padChar) + data);
    }

    // Method to pad the end of the string with a specified character (padEnd)
    SK_String_Base padEnd(size_t targetLength, char padChar = ' ') const {
        if (data.length() >= targetLength) return *this;
        return SK_String_Base(data + std::string(targetLength - data.length(), padChar));
    }

    // Method to repeat the string n times (repeat)
    SK_String_Base repeat(size_t count) const {
        std::string result;
        for (size_t i = 0; i < count; ++i) {
            result += data;
        }
        return SK_String_Base(result);
    }

    // Method to extract a substring from the string (slice)
    SK_String_Base slice(size_t start, size_t end) const {
        return SK_String_Base(data.substr(start, end - start));
    }

    // Method to extract a substring (substring)
    SK_String_Base substring(size_t start, size_t end) const {
        return SK_String_Base(data.substr(start, end - start));
    }

    // Method to convert the string to lowercase (toLowerCase)
    SK_String_Base toLowerCase() const {
        std::string result = data;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return SK_String_Base(result);
    }

    // Method to convert the string to uppercase (toUpperCase)
    SK_String_Base toUpperCase() const {
        std::string result = data;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return SK_String_Base(result);
    }

    // Method to trim whitespace from both ends (trim)
    SK_String_Base trim() const {
        size_t first = data.find_first_not_of(" \t\n\r");
        size_t last = data.find_last_not_of(" \t\n\r");
        if (first == std::string::npos || last == std::string::npos) return SK_String_Base("");
        return SK_String_Base(data.substr(first, (last - first + 1)));
    }

    // Method to split the string into a vector of strings based on a delimiter (split)
    std::vector<std::string> split(const SK_String_Base& delimiter) const {
        std::vector<std::string> result;
        std::string token;
        std::stringstream ss(data);
        char _delimiter = delimiter.data[0];
        while (std::getline(ss, token, _delimiter)) {
            result.push_back(SK_String_Base(token));
        }
        return result;
    }

    // Method to join a vector of strings into a single string (join)
    static SK_String_Base join(const std::vector<SK_String_Base>& arr, const std::string& separator = "") {
        std::string result;
        for (size_t i = 0; i < arr.size(); ++i) {
            result += arr[i].data;
            if (i < arr.size() - 1) {
                result += separator;
            }
        }
        return SK_String_Base(result);
    }

    // Method to replace the first occurrence of a pattern with a replacement (replace)
    SK_String_Base replace(const std::string& pattern, const std::string& replacement) const {
        std::string result = data;  // Create a copy of the string data
        size_t pos = 0;

        // Loop to replace all occurrences of the pattern
        while ((pos = result.find(pattern, pos)) != std::string::npos) {
            result.replace(pos, pattern.length(), replacement);
            pos += replacement.length();  // Move past the last replaced part
        }

        return SK_String_Base(result);  // Return a new SK_String_Base with the modified resul
    }

    // Method to replace all occurrences of a pattern with a replacement (replaceAll)
    SK_String_Base replaceAll(const std::string& pattern, const std::string& replacement) const {
        std::string result = data;
        std::string::size_type pos = 0;
        while ((pos = result.find(pattern, pos)) != std::string::npos) {
            result.replace(pos, pattern.length(), replacement);
            pos += replacement.length();
        }
        return SK_String_Base(result);
    }

    // Method to check if the string matches a regular expression pattern (match)
    std::smatch match(const std::string& pattern) const {
        std::smatch res;
        std::regex_match(data, res, std::regex(pattern));
        return res;
    }


    const char* c_str() {
        return data.c_str();
    }

    // Method to get the raw string
    const std::string& raw() const {
        return data;
    }

    // Method to convert the object back to std::string
    operator std::string() const {
        return data;
    }

    // Method to get the raw string
    operator const std::filesystem::path () const {
        return data;
    }

    // Method to get the raw string
    explicit operator const char* () const {
        return data.c_str();
    }





    // Operator + for SK_String_Base + SK_String_Base
    SK_String_Base operator+(const char* other) const {
        return SK_String_Base(data + other);
    }

    // Operator + for SK_String_Base + SK_String_Base
    SK_String_Base operator+(const SK_String_Base& other) const {
        return SK_String_Base(data + other.data);
    }

    // Operator + for SK_String_Base + std::string
    SK_String_Base operator+(const std::string& other) const {
        return SK_String_Base(data + other);
    }

    // Operator + for std::string + SK_String_Base
    friend SK_String_Base operator+(const std::string& lhs, const SK_String_Base& rhs) {
        return SK_String_Base(lhs + rhs.data);
    }

    // Operator+= for SK_String_Base
    SK_String_Base& operator+=(const SK_String_Base& other) {
        data += other.data;
        return *this;
    }

    // Operator+= for std::string
    SK_String_Base& operator+=(const std::string& other) {
        data += other;
        return *this;
    }

    // Operator+= for const char*
    SK_String_Base& operator+=(const char* other) {
        data += other;
        return *this;
    }



    // Operator== for SK_String_Base
    bool operator==(const SK_String_Base& other) const {
        return data == other.data;
    }

    // Operator== for std::string
    bool operator==(const std::string& other) const {
        return data == other;
    }

    // Operator== for const char*
    bool operator==(const char* other) const {
        return data == other;
    }

    // Friend functions to allow comparisons with SK_String_Base on the right-hand side

    // std::string == SK_String_Base
    friend bool operator==(const std::string& lhs, const SK_String_Base& rhs) {
        return lhs == rhs.data;
    }

    // const char* == SK_String_Base
    friend bool operator==(const char* lhs, const SK_String_Base& rhs) {
        return lhs == rhs.data;
    }




    // Operator!= for SK_String_Base
    bool operator!=(const SK_String_Base& other) const {
        return data != other.data;
    }

    // Operator!= for std::string
    bool operator!=(const std::string& other) const {
        return data != other;
    }

    // Operator!= for const char*
    bool operator!=(const char* other) const {
        return data != other;
    }

    // Friend functions to allow comparisons with SK_String_Base on the right-hand side

    // std::string != SK_String_Base
    friend bool operator!=(const std::string& lhs, const SK_String_Base& rhs) {
        return lhs != rhs.data;
    }

    // const char* != SK_String_Base
    friend bool operator!=(const char* lhs, const SK_String_Base& rhs) {
        return lhs != rhs.data;
    }


    
    friend std::ostream& operator<<(std::ostream& os, const SK_String_Base& skStr) {
        os << skStr.data;
        return os;
    }



    static inline const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    
    bool is_base64_char(unsigned char c) {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || (c == '+') || (c == '/');
    }

    std::string toBase64() {
        unsigned char const* bytes_to_encode = reinterpret_cast<const unsigned char*>(data.c_str());
        unsigned int in_len = data.length();
        std::string ret;
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3], char_array_4[4];

        while (in_len--) {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) | ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) | ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; i < 4; i++)
                    ret += base64_chars[char_array_4[i]];

                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) | ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) | ((char_array_3[2] & 0xc0) >> 6);

            for (j = 0; j < i + 1; j++)
                ret += base64_chars[char_array_4[j]];

            while ((i++ < 3))
                ret += '=';
        }

        return ret;
    }

    SK_String_Base fromBase64() {
        int in_len = data.size();
        int i = 0, j = 0, in_ = 0;
        unsigned char char_array_4[4], char_array_3[3];
        std::string ret;

        while (in_len-- && (data[in_] != '=') && is_base64_char(data[in_])) {
            char_array_4[i++] = data[in_];
            in_++;
            if (i == 4) {
                for (i = 0; i < 4; i++)
                    char_array_4[i] = base64_chars.find(char_array_4[i]);

                char_array_3[0] = (char_array_4[0] << 2) | (char_array_4[1] >> 4);
                char_array_3[1] = ((char_array_4[1] & 15) << 4) | (char_array_4[2] >> 2);
                char_array_3[2] = ((char_array_4[2] & 3) << 6) | char_array_4[3];

                for (i = 0; (i < 3); i++)
                    ret += char_array_3[i];
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 4; j++)
                char_array_4[j] = 0;

            for (j = 0; j < 4; j++)
                char_array_4[j] = base64_chars.find(char_array_4[j]);

            char_array_3[0] = (char_array_4[0] << 2) | (char_array_4[1] >> 4);
            char_array_3[1] = ((char_array_4[1] & 15) << 4) | (char_array_4[2] >> 2);
            char_array_3[2] = ((char_array_4[2] & 3) << 6) | char_array_4[3];

            for (j = 0; (j < i - 1); j++)
                ret += char_array_3[j];
        }

        return ret;
    }
};

END_SK_NAMESPACE
