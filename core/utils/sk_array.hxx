#pragma once

#include "../sk_common.hxx"


BEGIN_SK_NAMESPACE


template<typename T>
class SK_Array {
public:
    std::vector<T> data;

    // Constructor
    SK_Array() {}
    SK_Array(const std::vector<T>& v) : data(v) {}
    SK_Array(const SK_Array& v) : data(v.data) {}

    void fromStringVector(const std::vector<std::string>& v) {
        for (const auto& str : v) {
            data.push_back(SK_String(str)); // Assuming T can be constructed from std::string
        }
    }

    
    // Add a value to the end
    void push(const T& value) {
        data.push_back(value);
    }

    // Pop a value from the end
    T pop() {
        if (data.empty()) return T();
        T value = data.back();
        data.pop_back();
        return value;
    }

    // Remove the first element
    T shift() {
        if (data.empty()) return T();
        T value = data.front();
        data.erase(data.begin());
        return value;
    }

    // Add value to the beginning
    void unshift(const T& value) {
        data.insert(data.begin(), value);
    }

    // Concatenate arrays
    SK_Array concat(const SK_Array& other) const {
        SK_Array result = *this;
        result.data.insert(result.data.end(), other.data.begin(), other.data.end());
        return result;
    }

    // Join elements into a single string with a delimiter
    std::string join(const std::string& delimiter = ",") const {
        if (data.empty()) return "";
        std::ostringstream oss;
        oss << data.front();
        for (size_t i = 1; i < data.size(); ++i) {
            oss << delimiter << data[i];
        }
        return oss.str();
    }

    // ForEach: Apply a function to each element
    void forEach(std::function<void(T&)> func) {
        for (auto& element : data) {
            func(element);
        }
    }

    // Map: Transform each element
    SK_Array map(std::function<T(const T&)> func) const {
        SK_Array result;
        result.data.reserve(data.size());
        for (const auto& element : data) {
            result.push(func(element));
        }
        return result;
    }

    // Filter: Select elements that pass the test
    SK_Array filter(std::function<bool(const T&)> func) const {
        SK_Array result;
        for (const auto& element : data) {
            if (func(element)) {
                result.push(element);
            }
        }
        return result;
    }

    // Reduce: Accumulate elements to a single value
    T reduce(std::function<T(T&, const T&)> func, const T& initial = T()) const {
        T result = initial;
        for (const auto& element : data) {
            result = func(result, element);
        }
        return result;
    }

    // Check if array contains a value
    bool includes(const T& value) const {
        return std::find(data.begin(), data.end(), value) != data.end();
    }

    // Find index of value
    int indexOf(const T& value) const {
        auto it = std::find(data.begin(), data.end(), value);
        return it != data.end() ? static_cast<int>(std::distance(data.begin(), it)) : -1;
    }

    // Sort the elements
    void sort(std::function<bool(const T&, const T&)> compare = std::less<T>()) {
        std::sort(data.begin(), data.end(), compare);
    }

    // Reverse the elements
    void reverse() {
        std::reverse(data.begin(), data.end());
    }

    // Slice: Get a sub-array from a range
    SK_Array slice(int begin, int end) const {
        SK_Array result;
        if (begin < 0) begin = data.size() + begin;
        if (end < 0) end = data.size() + end;
        if (begin < 0) begin = 0;
        if (end > data.size()) end = data.size();
        result.data.insert(result.data.end(), data.begin() + begin, data.begin() + end);
        return result;
    }

    // Splice: Remove and/or add elements to the array
    void splice(int index, int count, const T& value) {
        if (index < 0) index = data.size() + index;
        data.erase(data.begin() + index, data.begin() + index + count);
        data.insert(data.begin() + index, value);
    }

    // Check if array is empty
    bool empty() const {
        return data.empty();
    }

    // Get the first element
    T first() const {
        return data.empty() ? T() : data.front();
    }

    // Get the last element
    T last() const {
        return data.empty() ? T() : data.back();
    }

    // Length of the array
    size_t length() const {
        return data.size();
    }

    // Convert to a standard vector of the same type
    std::vector<T> toStdVector() const {
        return data;
    }

    // Static method to create an SK_Array from a list of values
    static SK_Array from(const std::initializer_list<T>& initList) {
        SK_Array array;
        array.data.insert(array.data.end(), initList.begin(), initList.end());
        return array;
    }

    // Static method to create an SK_Array from a range
    static SK_Array fromRange(const std::vector<T>& range, size_t start, size_t end) {
        SK_Array array;
        array.data.insert(array.data.end(), range.begin() + start, range.begin() + end);
        return array;
    }

    // Find the first element that satisfies a given condition
    T find(std::function<bool(const T&)> predicate) const {
        auto it = std::find_if(data.begin(), data.end(), predicate);
        return (it != data.end()) ? *it : T();
    }

    // Find all elements that satisfy a given condition
    SK_Array findAll(std::function<bool(const T&)> predicate) const {
        SK_Array result;
        std::copy_if(data.begin(), data.end(), std::back_inserter(result.data), predicate);
        return result;
    }

    // Remove all elements from the array
    void clear() {
        data.clear();
    }

    // Merge with another array
    SK_Array merge(const SK_Array& other) const {
        SK_Array result = *this;
        result.data.insert(result.data.end(), other.data.begin(), other.data.end());
        return result;
    }





    // Method to convert the object back to std::vector<T>
    operator std::vector<T>() const {
        return data;
    }

    // Method to convert the object back to std::vector<std::string> (special case)
    operator std::vector<std::string>() const {
        std::vector<std::string> result;
        for (const auto& item : data) {
            result.push_back(item);  // Assuming T can be converted to std::string
        }
        return result;
    }

    // Non-const version of the operator[] for mutable access
    T& operator[](size_t index) {
        return data[index];
    }

    // Const version of the operator[] for read-only access
    const T& operator[](size_t index) const {
        return data[index];
    }
};


END_SK_NAMESPACE