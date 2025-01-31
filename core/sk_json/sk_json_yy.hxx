#pragma once

#include "../sk_common.hxx"

BEGIN_SK_NAMESPACE

class SK_JSON_YY_Iterator {
public:
    SK_JSON_YY_Iterator(yyjson_mut_val* obj, bool is_end = false) {
        if (obj && yyjson_mut_is_obj(obj)) {
            yyjson_mut_obj_iter_init(obj, &iter);
            if (is_end) {
                // Move to the end
                while (yyjson_mut_obj_iter_next(&iter)) {}
            }
        }
    }

    // Dereference operator
    std::pair<std::string, yyjson_mut_val*> operator*() const {
        yyjson_mut_obj_iter _iter = iter;
        char* _key;
        yyjson_mut_val* key = yyjson_mut_obj_iter_get(&_iter, _key); // Get current key
        yyjson_mut_val* val = yyjson_mut_obj_iter_get_val(key); // Get current value
        return { yyjson_mut_get_str(key), val };
    }

    // Pre-increment operator
    SK_JSON_YY_Iterator& operator++() {
        yyjson_mut_obj_iter_next(&iter);
        return *this;
    }

    // Inequality operator
    bool operator!=(const SK_JSON_YY_Iterator& other) const {
        return iter.idx != other.iter.idx;
    }

private:
    yyjson_mut_obj_iter iter; // Iterator for JSON object
};

class SK_JSON_YY {
public:
    // Constructor for creating an empty JSON object
    SK_JSON_YY() {
        doc = yyjson_mut_doc_new(nullptr);
        root = yyjson_mut_obj(doc);
        yyjson_mut_doc_set_root(doc, root);
    }

    // Constructor for parsing a JSON string
    SK_JSON_YY(const std::string& jsonStr) {
        doc = yyjson_mut_doc_new(nullptr);
        yyjson_doc* read_doc = yyjson_read(jsonStr.c_str(), jsonStr.size(), 0);
        if (read_doc) {
            root = yyjson_val_mut_copy(doc, yyjson_doc_get_root(read_doc));
            yyjson_doc_free(read_doc);
        }
        else {
            root = yyjson_mut_obj(doc);
        }
        yyjson_mut_doc_set_root(doc, root);
    }

    SK_JSON_YY(std::initializer_list<std::pair<std::string, yyjson_mut_val*>> list) : SK_JSON_YY() {
        for (const auto& [key, val] : list) {
            yyjson_mut_obj_add(root, yyjson_mut_str(doc, key.c_str()), val);
        }
    }

    // Destructor
    ~SK_JSON_YY() {
        if (doc) {
            yyjson_mut_doc_free(doc);
        }
    }

    operator SK_String() const {
        if (current_val && yyjson_mut_is_str(current_val)) {
            return SK_String(yyjson_mut_get_str(current_val));
        }
        return SK_String(); // Return an empty SK_String if the value is not a string
    }

    // Access or create a key-value pair
    SK_JSON_YY operator[](const std::string& key) {
        yyjson_mut_val* val = yyjson_mut_obj_get(root, key.c_str());
        if (!val) {
            // Add a null value for the key
            bool success = yyjson_mut_obj_add_null(doc, root, key.c_str());
            if (success) {
                // Retrieve the newly added value
                val = yyjson_mut_obj_get(root, key.c_str());
            }
            else {
                // Handle error (e.g., throw an exception or return a default value)
                throw std::runtime_error("Failed to add key to JSON object");
            }
        }
        return SK_JSON_YY(doc, val);
    }


    // Assign a value to a key
    SK_JSON_YY& operator=(const std::string& value) {
        if (current_val) {
            // Create a new string value
            yyjson_mut_val* new_val = yyjson_mut_str(doc, value.c_str());
            if (new_val) {
                // Replace the current value with the new string value
                yyjson_mut_obj_replace(root, current_val, new_val);
            }
        }
        return *this;
    }

    // Convert to string
    std::string dump(int indents = 0) const {
        const char* json_str = yyjson_mut_write(doc, 0, nullptr);
        std::string result(json_str ? json_str : "");
        if (json_str) {
            free((void*)json_str);
        }
        return result;
    }


    bool contains(const SK_String& key) const {
        if (root && yyjson_mut_is_obj(root)) {
            return yyjson_mut_obj_get(root, key.data.c_str()) != nullptr;
        }
        return false;
    }



    // Iterator begin
    SK_JSON_YY_Iterator begin() const {
        return SK_JSON_YY_Iterator(root);
    }

    // Iterator end
    SK_JSON_YY_Iterator end() const {
        return SK_JSON_YY_Iterator(root, true);
    }

    static SK_JSON_YY parse(const std::string& jsonStr) {
        yyjson_doc* read_doc = yyjson_read(jsonStr.c_str(), jsonStr.size(), 0);
        if (!read_doc) {
            throw std::runtime_error("Failed to parse JSON string");
        }

        // Create a mutable copy of the parsed document
        yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
        yyjson_mut_val* root = yyjson_val_mut_copy(doc, yyjson_doc_get_root(read_doc));
        yyjson_mut_doc_set_root(doc, root);

        // Free the read-only document
        yyjson_doc_free(read_doc);

        return SK_JSON_YY(doc, root);
    }
private:
    // Private constructor for internal use
    SK_JSON_YY(yyjson_mut_doc* doc, yyjson_mut_val* val) : doc(doc), current_val(val) {}

    yyjson_mut_doc* doc = nullptr;
    yyjson_mut_val* root = nullptr;
    yyjson_mut_val* current_val = nullptr;
};

END_SK_NAMESPACE
