#pragma once

#include "../sk_common.hxx"

BEGIN_SK_NAMESPACE

class SK_String {
private:

public:
    std::string data;

    // Constructor
    SK_String() : data("") {}
    SK_String(const char *s) : data(s) {}
    SK_String(const std::string& s) : data(s) {}
    SK_String(const SK_String& s) : data(s) {}

    #if defined(SK_OS_windows)
        SK_String(LPCWSTR wideStr) {
            if (wideStr == nullptr) {
                data = ""; // Handle null pointers safely
            }
            else {
                // Get the required buffer size for the conversion
                int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, nullptr, 0, nullptr, nullptr);

                if (bufferSize == 0) {
                    data = ""; // Conversion failed
                }
                else {
                    // Allocate a buffer for the converted string
                    std::string result(bufferSize - 1, '\0'); // -1 to exclude the null terminator

                    // Perform the conversion
                    WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, result.data(), bufferSize, nullptr, nullptr);

                    data = result; // Assign to the `data` member
                }
            }
        }
    #endif

    /*
    SK_String(const std::int8_t n) : data(std::to_string(n)) {}
    SK_String(const std::uint8_t n) : data(std::to_string(n)) {}
    SK_String(const std::int16_t n) : data(std::to_string(n)) {}
    SK_String(const std::uint16_t n) : data(std::to_string(n)) {}
    SK_String(const std::int32_t n) : data(std::to_string(n)) {}
    SK_String(const std::uint32_t n) : data(std::to_string(n)) {}
    SK_String(const std::int64_t n) : data(std::to_string(n)) {}
    SK_String(const std::uint64_t n) : data(std::to_string(n)) {}
    SK_String(const long n) : data(std::to_string(n)) {}
    SK_String(const unsigned long n) : data(std::to_string(n)) {}
    SK_String(const long long n) : data(std::to_string(n)) {}
    SK_String(const unsigned long long n) : data(std::to_string(n)) {}

    SK_String(const float& n, const int& precision) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << n;
        data = oss.str();
    }

    SK_String(const double& n, const int& precision) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << n;
        data = oss.str();
    }

    SK_String(const long double& n, const int& precision) {
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
    static SK_String concat(const SK_String& str1, const SK_String& str2) {
        return SK_String(str1.data + str2.data);
    }

    // Method to pad the start of the string with a specified character (padStart)
    SK_String padStart(size_t targetLength, char padChar = ' ') const {
        if (data.length() >= targetLength) return *this;
        return SK_String(std::string(targetLength - data.length(), padChar) + data);
    }

    // Method to pad the end of the string with a specified character (padEnd)
    SK_String padEnd(size_t targetLength, char padChar = ' ') const {
        if (data.length() >= targetLength) return *this;
        return SK_String(data + std::string(targetLength - data.length(), padChar));
    }

    // Method to repeat the string n times (repeat)
    SK_String repeat(size_t count) const {
        std::string result;
        for (size_t i = 0; i < count; ++i) {
            result += data;
        }
        return SK_String(result);
    }

    // Method to extract a substring from the string (slice)
    SK_String slice(size_t start, size_t end) const {
        return SK_String(data.substr(start, end - start));
    }

    // Method to extract a substring (substring)
    SK_String substring(size_t start, size_t end) const {
        return SK_String(data.substr(start, end - start));
    }

    // Method to convert the string to lowercase (toLowerCase)
    SK_String toLowerCase() const {
        std::string result = data;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return SK_String(result);
    }

    // Method to convert the string to uppercase (toUpperCase)
    SK_String toUpperCase() const {
        std::string result = data;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return SK_String(result);
    }

    // Method to trim whitespace from both ends (trim)
    SK_String trim() const {
        size_t first = data.find_first_not_of(" \t\n\r");
        size_t last = data.find_last_not_of(" \t\n\r");
        if (first == std::string::npos || last == std::string::npos) return SK_String("");
        return SK_String(data.substr(first, (last - first + 1)));
    }

    // Method to split the string into a vector of strings based on a delimiter (split)
    std::vector<std::string> split(const SK_String& delimiter) const {
        std::vector<std::string> result;
        std::string token;
        std::stringstream ss(data);
        char _delimiter = delimiter[0];
        while (std::getline(ss, token, _delimiter)) {
            result.push_back(SK_String(token));
        }
        return result;
    }

    // Method to join a vector of strings into a single string (join)
    static SK_String join(const std::vector<SK_String>& arr, const std::string& separator = "") {
        std::string result;
        for (size_t i = 0; i < arr.size(); ++i) {
            result += arr[i].data;
            if (i < arr.size() - 1) {
                result += separator;
            }
        }
        return SK_String(result);
    }

    // Method to replace the first occurrence of a pattern with a replacement (replace)
    SK_String replace(const std::string& pattern, const std::string& replacement) const {
        std::string result = data;  // Create a copy of the string data
        size_t pos = 0;

        // Loop to replace all occurrences of the pattern
        while ((pos = result.find(pattern, pos)) != std::string::npos) {
            result.replace(pos, pattern.length(), replacement);
            pos += replacement.length();  // Move past the last replaced part
        }

        return SK_String(result);  // Return a new SK_String with the modified resul
    }

    // Method to replace all occurrences of a pattern with a replacement (replaceAll)
    SK_String replaceAll(const std::string& pattern, const std::string& replacement) const {
        std::string result = data;
        std::string::size_type pos = 0;
        while ((pos = result.find(pattern, pos)) != std::string::npos) {
            result.replace(pos, pattern.length(), replacement);
            pos += replacement.length();
        }
        return SK_String(result);
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

    #if defined(SK_OS_windows)
        operator const LPCWSTR () const {
            static thread_local std::wstring wideString; // Thread-local to avoid issues with temporary scope
            int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, data.c_str(), -1, nullptr, 0);
            wideString.resize(sizeNeeded - 1); // Exclude null terminator
            MultiByteToWideChar(CP_UTF8, 0, data.c_str(), -1, &wideString[0], sizeNeeded);
            return wideString.c_str();
        }
    #endif

    // Method to get the raw string
    explicit operator const char* () const {
        return data.c_str();
    }





    // Operator + for SK_String + SK_String
    SK_String operator+(const char* other) const {
        return SK_String(data + other);
    }

    // Operator + for SK_String + SK_String
    SK_String operator+(const SK_String& other) const {
        return SK_String(data + other.data);
    }

    // Operator + for SK_String + std::string
    SK_String operator+(const std::string& other) const {
        return SK_String(data + other);
    }

    // Operator + for std::string + SK_String
    friend SK_String operator+(const std::string& lhs, const SK_String& rhs) {
        return SK_String(lhs + rhs.data);
    }

    // Operator+= for SK_String
    SK_String& operator+=(const SK_String& other) {
        data += other.data;
        return *this;
    }

    // Operator+= for std::string
    SK_String& operator+=(const std::string& other) {
        data += other;
        return *this;
    }

    // Operator+= for const char*
    SK_String& operator+=(const char* other) {
        data += other;
        return *this;
    }



    // Operator== for SK_String
    bool operator==(const SK_String& other) const {
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

    // Friend functions to allow comparisons with SK_String on the right-hand side

    // std::string == SK_String
    friend bool operator==(const std::string& lhs, const SK_String& rhs) {
        return lhs == rhs.data;
    }

    // const char* == SK_String
    friend bool operator==(const char* lhs, const SK_String& rhs) {
        return lhs == rhs.data;
    }




    // Operator!= for SK_String
    bool operator!=(const SK_String& other) const {
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

    // Friend functions to allow comparisons with SK_String on the right-hand side

    // std::string != SK_String
    friend bool operator!=(const std::string& lhs, const SK_String& rhs) {
        return lhs != rhs.data;
    }

    // const char* != SK_String
    friend bool operator!=(const char* lhs, const SK_String& rhs) {
        return lhs != rhs.data;
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

    SK_String fromBase64() {
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