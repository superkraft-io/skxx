#pragma once

#include "../sk_common.hxx"

BEGIN_SK_NAMESPACE

// Define variant type to support multiple data types
using SK_Value = std::variant<std::nullptr_t, bool, int, double, std::string, class SK_JSON_Glaze, std::vector<class SK_Value>>;

// Forward declaration for SK_JSON_Glaze
class SK_JSON_Glaze;

// Register SK_Value with Glaze (custom serialization)
template <>
struct glz::meta<SK_Value> {
    static constexpr auto value = glz::custom{};
};

// SK_JSON_Glaze class with dynamic type and array support
class SK_JSON_Glaze {
public:
    std::unordered_map<std::string, SK_Value> data;

    // Operator[] for easy access
    SK_Value& operator[](const std::string& key) {
        return data[key];
    }

    // Check if key exists
    bool contains(const std::string& key) const {
        return data.find(key) != data.end();
    }

    // Serialize to JSON string
    std::string stringify(bool pretty = false) const {
        std::string json;
        glz::write_json(*this, json, { .indent = pretty ? 4 : 0 });
        return json;
    }

    // Deserialize from JSON string
    bool parse(const std::string& jsonStr) {
        return glz::read_json(*this, jsonStr);
    }

    // Type-safe getter with automatic conversion
    template <typename T>
    T get(const std::string& key, const T& defaultValue = T{}) const {
        if (contains(key)) {
            const auto& val = data.at(key);
            if (auto ptr = std::get_if<T>(&val)) {
                return *ptr;
            }
            else if constexpr (std::is_same_v<T, int>) {
                if (auto strPtr = std::get_if<std::string>(&val)) return std::stoi(*strPtr);
                if (auto dblPtr = std::get_if<double>(&val)) return static_cast<int>(*dblPtr);
            }
            else if constexpr (std::is_same_v<T, double>) {
                if (auto strPtr = std::get_if<std::string>(&val)) return std::stod(*strPtr);
                if (auto intPtr = std::get_if<int>(&val)) return static_cast<double>(*intPtr);
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                if (auto intPtr = std::get_if<int>(&val)) return std::to_string(*intPtr);
                if (auto dblPtr = std::get_if<double>(&val)) return std::to_string(*dblPtr);
            }
        }
        return defaultValue;
    }

    // Add array support (vector operations)
    std::vector<SK_Value>& getArray(const std::string& key) {
        return std::get<std::vector<SK_Value>>(data[key]);
    }

    void pushToArray(const std::string& key, const SK_Value& value) {
        if (!contains(key) || !std::holds_alternative<std::vector<SK_Value>>(data[key])) {
            data[key] = std::vector<SK_Value>{};
        }
        std::get<std::vector<SK_Value>>(data[key]).push_back(value);
    }
};

// Register SK_JSON_Glaze with Glaze
template <>
struct glz::meta<SK_JSON_Glaze> {
    using T = SK_JSON_Glaze;
    static constexpr auto value = glz::object("data", &T::data);
};

END_SK_NAMESPACE
