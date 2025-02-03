#pragma once

#include "../sk_common.hxx"

BEGIN_SK_NAMESPACE

class SK_Number {
public:
    // Constructor to initialize with a value (int, long, float, int64_t, uint64_t, double)
    SK_Number() : value(0) {} // Default constructor
    SK_Number(int v) : value(v) {}
    SK_Number(unsigned int v) : value(v) {}
    SK_Number(long v) : value(v) {}
    SK_Number(float v) : value(v) {}
    SK_Number(int64_t v) : value(v) {}
    SK_Number(uint64_t v) : value(v) {}
    SK_Number(double v) : value(v) {}

    // Constructor to initialize from nlohmann::json
    SK_Number(const nlohmann::json& j) {
        if (j.is_number_integer()) {
            value = j.get<int64_t>();
        }
        else if (j.is_number_unsigned()) {
            value = j.get<uint64_t>();
        }
        else if (j.is_number_float()) {
            value = j.get<double>();
        }
        else {
            throw std::runtime_error("JSON value is not a number");
        }
    }

    // Conversion operator to nlohmann::json
    operator nlohmann::json() const {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        }
        else if (std::holds_alternative<long>(value)) {
            return std::get<long>(value);
        }
        else if (std::holds_alternative<float>(value)) {
            return std::get<float>(value);
        }
        else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value);
        }
        else if (std::holds_alternative<uint64_t>(value)) {
            return std::get<uint64_t>(value);
        }
        else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
        throw std::runtime_error("Invalid type in SK_Number");
    }

    // Assignment operators
    SK_Number& operator=(int v) {
        value = v;
        return *this;
    }

    SK_Number& operator=(unsigned int v) {
        value = v;
        return *this;
    }

    SK_Number& operator=(long v) {
        value = v;
        return *this;
    }

    SK_Number& operator=(float v) {
        value = v;
        return *this;
    }

    SK_Number& operator=(int64_t v) {
        value = v;
        return *this;
    }

    SK_Number& operator=(uint64_t v) {
        value = v;
        return *this;
    }

    SK_Number& operator=(double v) {
        value = v;
        return *this;
    }

    // Comparison operators
    bool operator==(int v) const {
        return std::holds_alternative<int>(value) && std::get<int>(value) == v;
    }

    bool operator==(unsigned int v) const {
        return std::holds_alternative<int>(value) && std::get<unsigned int>(value) == v;
    }

    bool operator==(long v) const {
        return std::holds_alternative<long>(value) && std::get<long>(value) == v;
    }

    bool operator==(float v) const {
        return std::holds_alternative<float>(value) && std::get<float>(value) == v;
    }

    bool operator==(int64_t v) const {
        return std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) == v;
    }

    bool operator==(uint64_t v) const {
        return std::holds_alternative<uint64_t>(value) && std::get<uint64_t>(value) == v;
    }

    bool operator==(double v) const {
        return std::holds_alternative<double>(value) && std::get<double>(value) == v;
    }

    bool operator==(const SK_Number& other) const {
        return value == other.value;
    }

    // Comparison operator for nlohmann::json
    bool operator==(const nlohmann::json& j) const {
        if (j.is_number_integer()) {
            return *this == j.get<int64_t>();
        }
        else if (j.is_number_unsigned()) {
            return *this == j.get<uint64_t>();
        }
        else if (j.is_number_float()) {
            return *this == j.get<double>();
        }
        return false; // Not a number
    }

    // Utility function to get the value as a specific type
    template <typename T>
    T get() const {
        if (std::holds_alternative<T>(value)) {
            return std::get<T>(value);
        }
        throw std::bad_variant_access(); // Throw if the type doesn't match
    }

    // Utility function to check the type
    template <typename T>
    bool holds() const {
        return std::holds_alternative<T>(value);
    }

    operator float() const {
        if (std::holds_alternative<int>(value)) {
            return static_cast<float>(std::get<int>(value));
        }
        else if (std::holds_alternative<long>(value)) {
            return static_cast<float>(std::get<long>(value));
        }
        else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<float>(std::get<int64_t>(value));
        }
        else if (std::holds_alternative<uint64_t>(value)) {
            return static_cast<float>(std::get<uint64_t>(value));
        }
        else if (std::holds_alternative<float>(value)) {
            return static_cast<float>(std::get<float>(value));
        }
        else if (std::holds_alternative<double>(value)) {
            return static_cast<float>(std::get<double>(value));
        }
        return 0; // Default value if no valid type is found
    }

    operator int() const {
        if (std::holds_alternative<int>(value)) {
            return static_cast<int>(std::get<int>(value));
        }
        else if (std::holds_alternative<long>(value)) {
            return static_cast<int>(std::get<long>(value));
        }
        else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<int>(std::get<int64_t>(value));
        }
        else if (std::holds_alternative<uint64_t>(value)) {
            return static_cast<int>(std::get<uint64_t>(value));
        }
        else if (std::holds_alternative<float>(value)) {
            return static_cast<int>(std::get<float>(value));
        }
        else if (std::holds_alternative<double>(value)) {
            return static_cast<int>(std::get<double>(value));
        }
        return 0; // Default value if no valid type is found
    }

#if defined(SK_OS_windows)
    operator LONG() const {
        if (std::holds_alternative<int>(value)) {
            return static_cast<LONG>(std::get<int>(value));
        }
        else if (std::holds_alternative<long>(value)) {
            return static_cast<LONG>(std::get<long>(value));
        }
        else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<LONG>(std::get<int64_t>(value));
        }
        else if (std::holds_alternative<uint64_t>(value)) {
            return static_cast<LONG>(std::get<uint64_t>(value));
        }
        else if (std::holds_alternative<float>(value)) {
            return static_cast<LONG>(std::get<float>(value));
        }
        else if (std::holds_alternative<double>(value)) {
            return static_cast<LONG>(std::get<double>(value));
        }
        return 0; // Default value if no valid type is found
    }


    operator BYTE() const {
        if (std::holds_alternative<int>(value)) {
            return static_cast<BYTE>(std::get<int>(value));
        }
        else if (std::holds_alternative<long>(value)) {
            return static_cast<BYTE>(std::get<long>(value));
        }
        else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<BYTE>(std::get<int64_t>(value));
        }
        else if (std::holds_alternative<uint64_t>(value)) {
            return static_cast<BYTE>(std::get<uint64_t>(value));
        }
        else if (std::holds_alternative<float>(value)) {
            return static_cast<BYTE>(std::get<float>(value));
        }
        else if (std::holds_alternative<double>(value)) {
            return static_cast<BYTE>(std::get<double>(value));
        }
        return 0; // Default value if no valid type is found
    }
#endif



    static inline SK_Number map(float value, float aMin, float aMax, float bMin, float bMax) {
        return (value - aMin) * (bMax - bMin) / (aMax - aMin) + bMin;
    }

    static inline SK_Number clamp(float value, float min, float max) {
        float clamped = value;
        if (clamped < min) clamped = min;
        if (clamped > max) clamped = max;
        return clamped;
    }
private:
    std::variant<int, unsigned int, long, float, int64_t, uint64_t, double> value;
};

END_SK_NAMESPACE
