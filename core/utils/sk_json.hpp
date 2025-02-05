#pragma once
/*
#include <string>
#include <unordered_map>
#include <any>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <memory>

#include "../sk_var.hpp"


BEGIN_SK_NAMESPACE
/*
class SK_JSON {
private:
    std::unordered_map<SK_String, ValueType> data;

public:
    using ValueType = std::any;

    // Constructor for initializer list
    SK_JSON(std::initializer_list<std::pair<SK_String, ValueType>> init) {
        for (const auto& pair : init) {
            data[pair.first] = pair.second;
        }
    }

    // Accessor operator with type casting
    template <typename T>
    T get(const SK_String& key) const {
        if (data.find(key) == data.end()) {
            throw std::runtime_error("Key not found: " + key);
        }

        const ValueType& value = data.at(key);
        try {
            return std::any_cast<T>(value);
        }
        catch (const std::bad_any_cast&) {
            throw std::runtime_error("Type mismatch for key: " + key);
        }
    }

    // Overloaded operator[] for read/write access
    ValueType& operator[](const SK_String& key) {
        return data[key];
    }

    const ValueType& operator[](const SK_String& key) const {
        return data.at(key);
    }

    // Stringify method
    SK_String stringify() const {
        std::ostringstream oss;
        oss << "{";
        bool first = true;
        for (const auto& [key, value] : data) {
            if (!first) oss << ",";
            first = false;

            std::ostringstream entry_oss;
            entry_oss << "\"" << key << "\":";

            if (value.type() == typeid(SK_String)) {
                entry_oss << "\"" << std::any_cast<SK_String>(value) << "\"";
            }
            else if (value.type() == typeid(int)) {
                entry_oss << std::any_cast<int>(value);
            }
            else if (value.type() == typeid(double)) {
                entry_oss << std::any_cast<double>(value);
            }
            else if (value.type() == typeid(bool)) {
                entry_oss << (std::any_cast<bool>(value) ? "true" : "false");
            }
            else if (value.type() == typeid(std::shared_ptr<SK_JSON>)) {
                entry_oss << std::any_cast<std::shared_ptr<SK_JSON>>(value)->stringify();
            }
            else {
                //do not add unsupported type
                continue;
            }

            oss << entry_oss;
        }
        oss << "}";
        return oss.str();
    }
};
*/
/*
END_SK_NAMESPACE
 */
