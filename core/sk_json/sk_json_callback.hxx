#pragma once

#include "../sk_common.hxx"

BEGIN_SK_NAMESPACE

class SK_JSON_Callback {
public:
    using SK_JSON_CB_onChanged = std::function<void(const SK_String& key)>;
    SK_JSON_CB_onChanged onChanged;
    bool bypassCallback = false;

    nlohmann::json data;

    SK_JSON_Callback() = default;

    SK_JSON_Callback(const nlohmann::json& json) : data(json) {}

    // Constructor for initializing from an initializer list
    SK_JSON_Callback(std::initializer_list<std::pair<SK_String, nlohmann::json>> initList) {
        for (const auto& pair : initList) {
            data[pair.first] = pair.second;
        }
    }

    class SK_JSON_Proxy {
    public:
        SK_JSON_Proxy(SK_JSON_Callback& parent, const std::string& key)
            : parent(parent), key(key), accessed(false) {}

        // Assignment operator
        SK_JSON_Proxy& operator=(const nlohmann::json& value) {
            accessed = true; // Track that the key was accessed and modified
            parent.data[key] = value;
            if (parent.bypassCallback == false && parent.onChanged) {
                parent.onChanged(key); // Trigger callback on modification
            }
            return *this;
        }

        operator int() const {
            if (parent.data.contains(key) && parent.data[key].is_number_integer()) {
                return parent.data[key].get<int>();
            }
            return 0; // Default value if the key doesn't exist or isn't an int
        }

        operator long() const {
            if (parent.data.contains(key) && parent.data[key].is_number_integer()) {
                return static_cast<long>(parent.data[key].get<int>());
            }
            return 0; // Default value if the key doesn't exist or isn't an int
        }

        operator float() const {
            if (parent.data.contains(key) && parent.data[key].is_number_float()) {
                return parent.data[key].get<float>();
            }
            return 0.; // Default value if the key doesn't exist or isn't an int
        }

        operator unsigned int() const {
            if (parent.data.contains(key) && parent.data[key].is_number_unsigned()) {
                return parent.data[key].get<unsigned int>();
            }
            return 0; // Default value if the key doesn't exist or isn't an int
        }

        operator std::string() const {
            if (parent.data.contains(key) && parent.data[key].is_string()) {
                return parent.data[key].get<std::string>();
            }
            return ""; // Default value if the key doesn't exist or isn't an int
        }
        
        operator SK_String() const {
            if (parent.data.contains(key) && parent.data[key].is_string()) {
                return SK_String(parent.data[key].get<std::string>());
            }
            return ""; // Default value if the key doesn't exist or isn't an int
        }
        
        #if defined(SK_OS_macos) || defined(SK_OS_ios)
            operator NSString*() const {
                if (parent.data.contains(key) && parent.data[key].is_string()) {
                    return [NSString stringWithUTF8String:parent.data[key].get<std::string>().c_str()];
                }
                return @""; // Default value if the key doesn't exist or isn't an int
            }
        #endif

        operator nlohmann::json() const {
            if (parent.data.contains(key) && parent.data[key].is_object()) {
                return parent.data[key];
            }
            return NULL; // Default value if the key doesn't exist or isn't an int
        }

        // Conversion operator for reading the value
        operator nlohmann::json() {
            accessed = true; // Track that the key was accessed
            if (parent.data.contains(key)) {
                return parent.data.at(key);
            }
            return nullptr; // Return null if the key doesn't exist
        }

        operator bool() const {
            if (parent.data.contains(key)) {
                return !parent.data[key].empty(); // Consider empty/null as "false"
            }
            return false; // Default to "false" if key doesn't exist
        }

        // Logical NOT operator
        bool operator!() const {
            return !(bool(*this)); // Use the `bool` conversion operator
        }
        // Track if the key was accessed
        bool wasAccessed() const {
            return accessed;
        }


        operator SK_Number() const {

            if (parent.data.contains(key)) {
                if (parent.data[key].is_number_integer()) {
                    return parent.data[key].get<int>(); // Extract integer
                }
                else if (parent.data[key].is_number_float()) {
                    return parent.data[key].get<double>(); // Extract floating-point
                }
                else if (parent.data[key].is_number_unsigned()) {
                    return parent.data[key].get<unsigned int>(); // Extract unsigned integer
                }
            }
            return SK_Number{}; // Default value if the key doesn't exist or isn't a number
        }

        // Assignment operator for SK_Number
        SK_JSON_Proxy& operator=(const SK_Number & value) {
            accessed = true; // Track that the key was accessed and modified
            parent.data[key] = value; // Assuming SK_Number can be directly assigned to nlohmann::json
            if (parent.bypassCallback == false && parent.onChanged) {
                parent.onChanged(key); // Trigger callback on modification
            }
            return *this;
        }

        float operator*(float scale) const {
            if (parent.data.contains(key) && parent.data[key].is_number()) {
                return parent.data[key].get<float>() * scale;
            }
            return 0.0f; // Default value if the key doesn't exist or isn't a number
        }


        template <typename T>
        auto operator+(T value) const {
            // Convert the proxy to a numeric type (e.g., int, float) and perform the addition
            if (parent.data.contains(key) && parent.data[key].is_number()) {
                return static_cast<T>(parent.data[key].get<T>()) + value;
            }
            return value; // Default behavior if the key doesn't exist or isn't a number
        }

        // Operator for int
        bool operator>(int value) const {
            if (parent.data.contains(key) && parent.data[key].is_number_integer()) {
                return parent.data[key].get<int>() > value;
            }
            return false; // Default behavior if the key doesn't exist or isn't an integer
        }

        // Operator for float
        bool operator>(float value) const {
            if (parent.data.contains(key) && parent.data[key].is_number_float()) {
                return parent.data[key].get<float>() > value;
            }
            return false; // Default behavior if the key doesn't exist or isn't a float
        }

        bool operator==(bool value) const {
            if (parent.data.contains(key) && parent.data[key].is_boolean()) {
                return parent.data[key].get<bool>() == value;
                if (parent.bypassCallback == false && parent.onChanged) {
                    parent.onChanged(key); // Trigger callback on modification
                }
            }
            return false; // Default behavior if the key doesn't exist or isn't a boolean
        }

        SK_JSON_Proxy& operator=(bool value) {
            accessed = true; // Track that the key was accessed and modified
            parent.data[key] = value; // Assign the bool value to the JSON key
            if (parent.bypassCallback == false && parent.onChanged) {
                parent.onChanged(key); // Trigger callback on modification
            }
            return *this;
        }
    private:
        SK_JSON_Callback& parent;
        std::string key;
        mutable bool accessed;
    };


    SK_JSON_Proxy operator[](const SK_String& key) {
        return SK_JSON_Proxy(*this, key);
    }

    void combineWith(const nlohmann::json& json) {
        data.update(json);
        if (bypassCallback == false && onChanged) onChanged("");
    }

private:
};

END_SK_NAMESPACE
