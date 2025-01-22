#pragma once

#include "../sk_common.hxx"

BEGIN_SK_NAMESPACE


inline size_t WriteCallback(char* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}


class SK_CURL_Request {
public:
    typedef size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);
    
            
    nlohmann::json call(const nlohmann::json& opt) {
       // Initialize the curl library
       curl_global_init(CURL_GLOBAL_DEFAULT);

       // Create a curl handle
       CURL* handle = curl_easy_init();

       try {

           #if defined(SK_MODE_DEBUG)
               curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, false);
               curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, false);
           #endif

           // Set headers
           struct curl_slist* headers = NULL;


           headers = curl_slist_append(headers, "charset: utf-8");
           SK_String mimeTypeStr;

           if (opt.contains("mimeType")) mimeTypeStr = SK_String(opt["mimeType"]);

           SK_String mimeType = SK_Web_MIME_utils::fromFileExt("foo." + mimeTypeStr);
           SK_String hStr = "Accept: " + mimeType;
           headers = curl_slist_append(headers, hStr.c_str());
           hStr = "Content-Type: " + mimeType;
           headers = curl_slist_append(headers, hStr.c_str());


           for (const std::string& item : opt["headers"]) {
               headers = curl_slist_append(headers, item.c_str());
           }

           curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);


           // Set the URL to request
           SK_String url = opt["url"];
           curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

           // Set the request method to POST
           SK_String type = opt["type"];
           curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, type.c_str());

           // Set the request body
           SK_String body;
           if (opt.contains("body") && !opt["body"].is_null() && opt["body"].size() > 0) {
               body = SK_String(opt["body"]);
           }
           else body = "{}";
           curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, (long)strlen(body.c_str()));
           curl_easy_setopt(handle, CURLOPT_POSTFIELDS, body.c_str());
           //curl_easy_setopt(handle, CURLOPT_POST, 1);

           // Set redirect count
           long redirects = 5;
           if (opt.contains("redirects") && !opt["redirects"].is_null() && opt["redirects"] > 0) {
               redirects = opt["redirects"];
           }
           curl_easy_setopt(handle, CURLOPT_MAXREDIRS, redirects);



           // Set the callback function to handle the response
           SK_String response;
           curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
           curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);


           // Perform the request
           CURLcode result = curl_easy_perform(handle);

           // Clean up
           curl_easy_cleanup(handle);
           curl_global_cleanup();

           // Check the result
           if (result != CURLE_OK) {
               SK_String err = curl_easy_strerror(result);
               return { {"error", err } };
           }



           return { {"error", nullptr}, {"data", response} };
       }
       catch (const std::bad_any_cast& e) {
           curl_easy_cleanup(handle);
           curl_global_cleanup();
           return { {"error", "failed web request"}};
       }
       
    };
};

class SK_CURL {
public:

    SK_CURL() {
    };

    nlohmann::json createRequest(const nlohmann::json&  opt) {//const String& url, String data, String& type = "GET", String mimeType);
        SK_CURL_Request request;
        return request.call(opt);
    };

    nlohmann::json get(nlohmann::json& opt) {
        opt["type"] = "GET";
        return createRequest(opt);
    };

    nlohmann::json post(nlohmann::json& opt) {
        opt["type"] = "POST";
        return createRequest(opt);
    };

    void onRequestCallback(const char* data) {
        // Handle the callback here
        std::cout << "Callback received data: " << data << std::endl;
    };

private:
};

END_SK_NAMESPACE
