#pragma once

#include "../../../../../core/sk_common.hxx"

BEGIN_SK_NAMESPACE

typedef struct {
    unsigned long id;
    nlohmann::json opt;
    nlohmann::json response;
} SK_WebRequestTask;

class SK_Module_web {
public:
    SK_CURL sk_curl;

    unsigned long taskIdx = 0;


    SK_Module_web() {

    };

    void handleOperation(const SK_String& operation, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
       // if (operation == "createProgressCallback") createProgressCallback(payload, respondWith);
        //else if (operation == "getProgress") getProgress(payload, respondWith);
        
             if (operation == "get") get(payload, respondWith);
        else if (operation == "post") post(payload, respondWith);
        else if (operation == "download") download(payload, respondWith);
    };

    void request(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        respondWith.async = true;

        SK_String url = payload["url"];

        //validate URL
        if (url == "") {
            respondWith.error(404, "invalid_url");
            return;
        }

        nlohmann::json body;
        if (!payload["body"].is_null()) body = payload["body"];

        SK_String mimeType = payload["mimeType"];

        taskIdx++;

        SK_WebRequestTask* reqTask = new SK_WebRequestTask();


        reqTask->id = taskIdx;

        reqTask->opt = {
            {"url", url},
            {"body", body},
            {"mimeType", mimeType},
            {"headers", payload["headers"]}
        };

        SK_Common::threadPool->newAsync([&, reqTask, respondWith](SK_Thread_Pool_MainThreadRunner runOnMainThread) mutable {
            reqTask->response = sk_curl.post(reqTask->opt);
            
            respondWith.JSON(reqTask->response);
            free(reqTask);
        });
    };

    void get(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
    
    };

    void post(const nlohmann::json& payload, SK_Communication_Response& respondWith) {
        request(payload, respondWith);
    };

    void download(const nlohmann::json& payload, SK_Communication_Response& respondWith) {

    };
};

END_SK_NAMESPACE