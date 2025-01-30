#pragma once

#include "sk_include.hxx"

static inline simdjson::dom::object convert_to_mutable(const simdjson::dom::object& immutable_obj) {
    simdjson::dom::object mutable_obj;

    // Iterate through key-value pairs in the immutable object
    for (auto kv : immutable_obj) {
        std::string_view key = kv.key();  // Get the key
        auto element = kv.value();  // Get the value

        if (element.is_object()) {
            // If it's an object, recursively convert to a mutable object
            mutable_obj[key] = convert_to_mutable(element);  // Recursive call
        }
        else if (element.is_array()) {
            // If it's an array, create a mutable array and copy the content
            simdjson::dom::array mutable_array;
            for (auto item : element) {
                mutable_array.push_back(item);  // Add each item to the array
            }
            mutable_obj[key] = mutable_array;
        }
        else {
            // Handle other types (string, number, etc.)
            mutable_obj[key] = element;
        }
    }

    return mutable_obj;
}