#pragma once

#include "../sk_common.hxx"

BEGIN_SK_NAMESPACE

class SK_JSON {
public:
    // Define a type to store various JSON-compatible values.
    using JSONValue = std::variant<std::nullptr_t, bool, int, double, std::string>;

    // Constructor to create an empty JSON object.
    SK_JSON() {
        doc = yyjson_mut_doc_new(nullptr);
        root = yyjson_mut_obj(doc);
        yyjson_mut_doc_set_root(doc, root);
    }

    // Destructor to clean up the document.
    ~SK_JSON() {
        yyjson_mut_doc_free(doc);
    }

    // Contains method to check if a key exists.
    bool contains(const std::string& key) const {
        yyjson_mut_val* value = yyjson_mut_obj_get(root, key.c_str());
        return value != nullptr;
    }

    // Overload the [] operator for getting/setting values.
    JSONValue operator[](const std::string& key) const {
        yyjson_mut_val* value = yyjson_mut_obj_get(root, key.c_str());
        if (!value) {
            throw std::out_of_range("Key does not exist: " + key);
        }

        switch (yyjson_mut_get_type(value)) {
        case YYJSON_TYPE_NULL:
            return nullptr;
        case YYJSON_TYPE_BOOL:
            return yyjson_mut_get_bool(value);
        case YYJSON_TYPE_NUM:
            return yyjson_mut_is_int(value) ? static_cast<int>(yyjson_mut_get_int(value)) : yyjson_mut_get_real(value);
        case YYJSON_TYPE_STR:
            return std::string(yyjson_mut_get_str(value));
        default:
            throw std::runtime_error("Unsupported type for key: " + key);
        }
    }

    // Serialize the JSON object to a string.
    std::string stringify() const {
        size_t length;
        const char* json_str = yyjson_mut_write(doc, YYJSON_WRITE_PRETTY, &length);
        std::string result(json_str, length);
        free((void*)json_str);
        return result;
    }

    // Parse a JSON string and populate the object.
    void parse(const std::string& json_str) {
        yyjson_doc* new_doc = yyjson_read(json_str.c_str(), json_str.length(), YYJSON_READ_NOFLAG);
        if (!new_doc) {
            throw std::runtime_error("Failed to parse JSON string.");
        }

        yyjson_mut_doc_free(doc); // Free the old document.
        doc = new_doc;
        root = yyjson_mut_doc_get_root(doc);
    }

private:
    yyjson_mut_doc* doc;
    yyjson_mut_val* root;
};


END_SK_NAMESPACE
