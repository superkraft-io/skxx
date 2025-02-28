#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.hpp"
//#include "../../libs/general/curl/curl.h"
#include "../sk_common.hxx"
#include "sk_curl.hxx"



using namespace SK;

std::vector<std::string> splitString(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos)
    {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }

    result.push_back(str.substr(start));

    return result;
}



typedef size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);
            

size_t WriteCallback(char* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

nlohmann::json SK_CURL_Request::call(const nlohmann::json& opt)  {
    /*
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
        std::string mimeTypeStr;

        try {
            mimeTypeStr = std::any_cast<std::string>(opt["mimeType"]);
        }
        catch (const std::bad_any_cast& e) {
            std::cerr << "Bad any_cast: " << e.what() << std::endl;
        }

        SK_String mimeType = SK_Web_MIME_utils::fromFileExt("foo." + mimeTypeStr);
        std::string hStr = "Accept: " + mimeType;
        headers = curl_slist_append(headers, hStr.c_str());
        hStr = "Content-Type: " + mimeType;
        headers = curl_slist_append(headers, hStr.c_str());

        std::string headersStr = std::any_cast<std::string>(opt["headers"]).c_str();


        std::vector<std::string> headersArr = splitString(headersStr, "<!-!>");
        for (const auto& item : headersArr) {
            //DBG(item);
            headers = curl_slist_append(headers, item.c_str());
        }

        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);


        // Set the URL to request
        std::string url = std::any_cast<std::string>(opt["url"]);
        curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

        // Set the request method to POST
        std::string type = std::any_cast<std::string>(opt["type"]);
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, type.c_str());

        // Set the request body
        std::string body = std::any_cast<std::string>(opt["body"]);
        curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, (long)strlen(body.c_str()));
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, body.c_str());
        //curl_easy_setopt(handle, CURLOPT_POST, 1);

        // Set redirect count
        long redirects = 5;
        redirects = std::any_cast<long>(opt["redirects"]);
        curl_easy_setopt(handle, CURLOPT_MAXREDIRS, redirects);
    
    

        // Set the callback function to handle the response

        std::string response;
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
    */

    return { {"error", "failed web request"} };
}






SK_CURL::SK_CURL() {
}

nlohmann::json SK_CURL::createRequest(const nlohmann::json& opt){
    SK_CURL_Request request;
    return request.call(opt);
}

nlohmann::json SK_CURL::get(nlohmann::json& opt) {
    opt["type"] = "GET";
    return createRequest(opt);
}

nlohmann::json SK_CURL::post(nlohmann::json& opt) {
    opt["type"] = "POST";
    return createRequest(opt);
}

void SK_CURL::onRequestCallback(const char* data) {
    // Handle the callback here
    std::cout << "Callback received data: " << data << std::endl;
}
