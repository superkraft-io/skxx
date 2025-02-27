#pragma once

#include "../sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Communication_Response;


#if defined(SK_OS_apple)
    #ifdef __OBJC__
        struct SK_Communicaton_Response_Apple {
            NSData* data;
            NSHTTPURLResponse* response;
        };
    #endif
#endif

using SK_Communication_Response_CB_packageIPCResponse = std::function<SK_String(const nlohmann::json& data)>;
using SK_Communication_Response_CB_onHandleResponse = std::function<void(SK_Communication_Response* response)>;



class SK_Communication_Response {
public:
	SK_Communication_Packet_Type type;
	SK_Communication_Config* config;

	using SK_Communication_Response_CB_setAsOK = std::function<void()>;
	using SK_Communication_Response_CB_JSON = std::function<bool(const nlohmann::json& json)>;
	using SK_Communication_Response_CB_JSON_OK = std::function<bool()>;
	using SK_Communication_Response_CB_string = std::function<bool(const SK_String& str, const SK_String& mimeType)>;
	using SK_Communication_Response_CB_file = std::function<bool(const SK_String& path, const SK_String& mimeType)>;
	using SK_Communication_Response_CB_error = std::function<void(int code, SK_String msg)>;

	SK_Communication_Response_CB_setAsOK CB_setAsOK;
	SK_Communication_Response_CB_JSON CB_JSON;
	SK_Communication_Response_CB_JSON_OK CB_JSON_OK;
	SK_Communication_Response_CB_string CB_string;
	SK_Communication_Response_CB_file CB_file;
	SK_Communication_Response_CB_error CB_error;

	SK_Communication_Response_CB_packageIPCResponse packageIPCResponse;

	using SK_Communication_Response_CB_getIPCResponse = std::function<SK_String()>;
	SK_Communication_Response_CB_getIPCResponse CB_getIPCResponse;

    #if defined(SK_OS_windows)
        using SK_Communication_Response_CB_getWebResponse = std::function<wil::com_ptr<ICoreWebView2WebResourceResponse>()>;
        SK_Communication_Response_CB_getWebResponse CB_getWebResponse;
    #elif defined(SK_OS_apple)
        #ifdef __OBJC__
            using SK_Communication_Response_CB_getWebResponse = std::function<SK_Communicaton_Response_Apple()>;
            SK_Communication_Response_CB_getWebResponse CB_getWebResponse;
        #endif
    #elif defined(SK_OS_linux) || defined(SK_OS_android)
        //for linux and android
    #endif
	SK_Communication_Response_CB_onHandleResponse onHandleResponse;

	bool async = false;


	void setAsOK() {
		CB_setAsOK();
	}

	bool JSON(const nlohmann::json& json) {
		bool res = CB_JSON(json);
		onHandleResponse(this);
		return res;
	}

	bool JSON_OK() {
		bool res = CB_JSON_OK();
		onHandleResponse(this);
		return res;
	}

	bool string(const SK_String& str, const SK_String& mimeType = "plain/text"){
		bool res = CB_string(str, mimeType);
		onHandleResponse(this);
		return res;
	}

	bool file(const SK_String& path, const SK_String& mimeType = "auto") {
		bool res = CB_file(path, mimeType);
		onHandleResponse(this);
		return res;
	}

	void error(int code = 404, SK_String msg = "Not Found") {
		CB_error(code, msg);
		onHandleResponse(this);
	}


	SK_String getForIPC() {
		return CB_getIPCResponse();
	}

#if defined(SK_OS_windows)
	wil::com_ptr<ICoreWebView2WebResourceResponse> getForWeb() {
		return CB_getWebResponse();
	}
#elif defined(SK_OS_apple)
    #ifdef __OBJC__
        SK_Communicaton_Response_Apple getForWeb() {
            return CB_getWebResponse();
        }
    #endif
#elif defined(SK_OS_linux) || defined(SK_OS_android)
	//for linux and android
#endif
};

class SK_Communication_Response_IPC : public SK_Communication_Response {
public:
	nlohmann::json data;

	SK_Communication_Response_IPC() {
		type = SK_Communication_Packet_Type::sk_comm_pt_ipc;

		CB_setAsOK = [&]() { setAsOK(); };
		CB_JSON = [&](nlohmann::json json) { return JSON(json); };
		CB_JSON_OK = [&]() { return JSON_OK(); };
		CB_string = [&](SK_String str, SK_String mimeType) { return string(str, mimeType); };
		CB_file = [&](SK_String path, SK_String mimeType) { return file(path, mimeType); };
		CB_error = [&](int code, SK_String msg) { error(code, msg); };

		CB_getIPCResponse = [&]() {
			return packageIPCResponse(data);
		};
	}

	void setAsOK() {
		data = nlohmann::json{};
	}

	bool JSON(const nlohmann::json& json) {
		data = json;
		return true;
	}

	bool JSON_OK() {
		return string("{}");
	}

	bool string(const SK_String& str, const SK_String& mimeType = "plain/text") {
		data = nlohmann::json{
				{"string", str}
		};

		return true;
	}

	bool file(const SK_String& path, const SK_String& mimeType = "auto") {
		std::vector<char> fileData;

		SK_File file;
		if (file.loadFromDisk(path.replaceAll("\\", "/").c_str())) {
			fileData = std::vector<char>(file.data.begin(), file.data.end());
			data = nlohmann::json {
				{"data", fileData}
			};

			return true;
		}

		error(); //something went wrong reading the file so we return a 404

		return false;
	}

	void error(int code = 404, SK_String msg = "Not Found") {
		data = nlohmann::json{ {"error", code}, {"message", msg} };
	}

};

class SK_Communication_Response_Web : public SK_Communication_Response {
public:

    SK_String errorType = "plain/text";
    int statusCode = 404;
    SK_String statusMessage = "Not found";
    nlohmann::json headers{ {"Content-Type", "application/json"} };
    std::vector<char> data;
    SK_String url;


    
    #if defined(SK_OS_windows)
        wil::com_ptr<ICoreWebView2WebResourceResponse> response;
    #elif defined(SK_OS_apple)
        //for apple
    #elif defined(SK_OS_linux) || defined(SK_OS_android)
        //for linux and android
    #endif
    
    SK_Communication_Response_Web(const SK_String& _url = "") {
        type = SK_Communication_Packet_Type::sk_comm_pt_web;
        
        url = _url;

        SK_String defaultData = "{\"error\":\"404\",\"message\":\"Not found\"}";
        data = std::vector<char>(defaultData.data.begin(), defaultData.data.end());

        CB_setAsOK = [&]() { setAsOK(); };
        CB_JSON = [&](nlohmann::json json) { return JSON(json); };
        CB_JSON_OK = [&]() { return JSON_OK(); };
        CB_string = [&](SK_String str, SK_String mimeType) { return string(str, mimeType); };
        CB_file = [&](SK_String path, SK_String mimeType) { return file(path, mimeType); };
        CB_error = [&](int code, SK_String msg) { error(code, msg); };

        #if defined(SK_OS_windows)
            wil::com_ptr<ICoreWebView2WebResourceResponse> response;
        #elif defined(SK_OS_apple)
            //for apple
        #elif defined(SK_OS_linux) || defined(SK_OS_android)
            //for linux and android
        #endif
        
        
        
        
        #if defined(SK_OS_windows)
            CB_getWebResponse = [&]() { return getWebResponse(); };
        #elif defined(SK_OS_apple)
            #ifdef __OBJC__
                CB_getWebResponse = [&]() {
                    return getWebResponse();
                };
            #endif
        #endif
        
    }

    ~SK_Communication_Response_Web() {
        
        #if defined(SK_OS_windows)
            response.reset();
        #elif defined(SK_OS_apple)
            //for apple
        #elif defined(SK_OS_linux) || defined(SK_OS_android)
            //for linux and android
        #endif
    }


    void setAsOK() {
        statusCode = 200;
        statusMessage = "OK";
    }

    bool JSON(const nlohmann::json& json) {
        SK_String resAsString = std::any_cast<nlohmann::json>(json).dump(0);
        if (resAsString == "null") {
            resAsString = "{}";
        }

        statusMessage = "";

        data = std::vector<char>(resAsString.data.begin(), resAsString.data.end());

        headers["Content-Type"] = "application/json";

        setAsOK();

        return true;
    }

    bool JSON_OK() {
        return string("{}", "application/json");
    }

    bool string(const SK_String& str, const SK_String& mimeType = "plain/text") {
        statusMessage = "";

        data = std::vector<char>(str.data.begin(), str.data.end());
        headers["Content-Type"] = mimeType;

        setAsOK();

        return true;
    }

    bool file(const SK_String& path, const SK_String& mimeType = "auto") {
        SK_File file;
        if (file.loadFromDisk(path.replaceAll("\\", "/").c_str())) {
            headers["Content-Type"] = (mimeType == "auto" ? file.mimeType : mimeType);
            data = std::vector<char>(file.data.begin(), file.data.end());
            setAsOK();
            return true;
        }

        error(); //something went wrong reading the file so we return a 404

        return false;
    }

    bool error(int code = 404, const SK_String& msg = "Not Found") {


        if (errorType == "json") {
            JSON({
                {"error", code},
                { "message", msg }
            });
        }
        else {
            string("");
            statusCode = code;
            statusMessage = msg;
        }
    }

    
    #if defined(SK_OS_windows)
        wil::com_ptr<ICoreWebView2WebResourceResponse> getWebResponse() {
            // Create a memory stream from the byte array using CreateStreamOnHGlobal
            HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, data.size());
            if (hGlobal == nullptr) {
                // Handle error if memory allocation fails
                return nullptr;
            }

            // Lock the global memory and copy data to it
            void* pData = GlobalLock(hGlobal);
            if (pData != nullptr) {
                memcpy(pData, data.data(), data.size());
                GlobalUnlock(hGlobal);
            }

            // Create an IStream from the global memory handle
            wil::com_ptr<IStream> contentStream;
            HRESULT hr = ::CreateStreamOnHGlobal(hGlobal, TRUE, &contentStream);
            if (FAILED(hr)) {
                // Handle the error properly if needed
                GlobalFree(hGlobal);
                return nullptr;
            }




            wil::unique_cotaskmem_string _statusMessage = stringToUniqueCoTaskMemString(statusMessage);

            SK_String _headers = jsonToDelimitedString(headers) + "\r\nAccess-Control-Allow-Origin: *\r\n";
            wil::unique_cotaskmem_string _responseHeaders = stringToUniqueCoTaskMemString(_headers);

            // Create the response with status code, headers, and content stream
            hr = config->webviewEnvironment->CreateWebResourceResponse(contentStream.get(),   // The stream containing the custom content
                statusCode,             // HTTP status code
                _statusMessage.get(),   // Status message
                _responseHeaders.get(), // Headers
                &response               // Output response
            );

            if (SUCCEEDED(hr)) {
                return response.get();
            }
        }
        
    #elif defined(SK_OS_apple)
        #ifdef __OBJC__
            SK_Communicaton_Response_Apple getWebResponse() {
                NSInteger _statusCode = statusCode;
                
                headers["Content-Length"] = data.size();
                
                SK_Communicaton_Response_Apple res {
                    SK_String(data),
                    [[NSHTTPURLResponse alloc]
                           initWithURL:url
                            statusCode:_statusCode
                           HTTPVersion:@"HTTP/1.1"
                          headerFields:getHeadersAsNSDictionary()
                    ]
                };
                
                return res;
            }
        
            NSDictionary* getHeadersAsNSDictionary(){
                NSMutableDictionary *nsDict = [NSMutableDictionary dictionary];
                
                if (headers.is_object()) {
                    for (auto it = headers.begin(); it != headers.end(); ++it) {
                        
                        SK_String _key = it.key();
                        SK_String _value = it.value();
                        
                        NSString *key = [NSString stringWithUTF8String: _key.c_str()];
                        NSString *value = [NSString stringWithUTF8String: _value.c_str()];
                        [nsDict setObject:value forKey:key];
                    }
                }
               
                return [nsDict copy];
            }
        #endif
    #elif defined(SK_OS_linux) || defined(SK_OS_android)
    //for linux and android
    #endif
};



END_SK_NAMESPACE
