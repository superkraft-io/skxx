#pragma once

#include "../../sk_common.hxx"

using namespace Microsoft::WRL;

BEGIN_SK_NAMESPACE

class SK_WebViewResourceHandler;
class SK_Window;

class SK_WebView {
public:
    using SK_WebView_EvaluationComplete_Callback = std::function<void(const SK_String& result)>;
    using SK_WebView_callResize_Callback = std::function<void()>;

    SK_WebViewResourceHandler* wvrh;

    SK_String parentClassName;

	HWND* parentHwnd;

    wil::com_ptr<ICoreWebView2Environment> environment;
	wil::com_ptr<ICoreWebView2Settings> settings;
	wil::com_ptr<ICoreWebView2Controller> controller = nullptr;
	wil::com_ptr<ICoreWebView2> webview = nullptr;
    EventRegistrationToken mWebMessageReceivedToken;
    
	SK_String currentURL = "";

    SK_WebView_callResize_Callback callResize;

    std::string ipcTestStr = "{\"L1_obj1\":{\"L2_str1\":\"another string - level 2 object of obj 1 at level 1 - but this is much longer\",\"L2_obj1\":{\"string\":\"another string but not as long\"}},\"L1_obj2\":{\"L2_str1ng\":\"short string\",\"L2_str1\":\"this is a very long string - this is a very long string - this is a very long string - this is a very long string - this is a very long string\",\"L2_obj1\":{\"string\":\"kind of a lonigsh string - this is a story all about how mynlife got flipped upside down\"}}}";



    nlohmann::json yyjsonToNlohmann(yyjson_mut_val* node) {
        if (!node) return nullptr;

        if (yyjson_mut_is_obj(node)) {
            // If the node is an object, iterate over its key-value pairs
            nlohmann::json obj = nlohmann::json::object();
            yyjson_mut_obj_iter iter;
            yyjson_mut_obj_iter_init(node, &iter);
            yyjson_mut_val* key;
            while ((key = yyjson_mut_obj_iter_next(&iter))) {
                yyjson_mut_val* val = yyjson_mut_obj_iter_get_val(key);
                std::string key_str = yyjson_mut_get_str(key);
                obj[key_str] = yyjsonToNlohmann(val); // Recursively convert value
            }
            return obj;
        }
        else if (yyjson_mut_is_arr(node)) {
            // If the node is an array, iterate over its elements
            nlohmann::json arr = nlohmann::json::array();
            size_t idx, max;
            yyjson_mut_val* val;
            yyjson_mut_arr_foreach(node, idx, max, val) {
                arr.push_back(yyjsonToNlohmann(val)); // Recursively convert element
            }
            return arr;
        }
        else if (yyjson_mut_is_str(node)) {
            // If the node is a string, return its value
            return yyjson_mut_get_str(node);
        }
        else if (yyjson_mut_is_int(node)) {
            // If the node is an integer, return its value
            return yyjson_mut_get_int(node);
        }
        else if (yyjson_mut_is_real(node)) {
            // If the node is a floating-point number, return its value
            return yyjson_mut_get_real(node);
        }
        else if (yyjson_mut_is_bool(node)) {
            // If the node is a boolean, return its value
            return yyjson_mut_get_bool(node);
        }
        else if (yyjson_mut_is_null(node)) {
            // If the node is null, return nullptr
            return nullptr;
        }

        // If the node type is unknown, return nullptr
        return nullptr;
    }



    void updateStyling(RECT rect) {
        //  !!! IMPORTANT !!!
        //  The following code must be executed in the exact order, or it won't work!

        //  1. Set the background color to transparent
        wil::com_ptr<ICoreWebView2Controller2> controller2 = controller.query<ICoreWebView2Controller2>(); //DO NOT TOUCH!
        COREWEBVIEW2_COLOR color = { 0, 0, 0, 0 }; //DO NOT TOUCH!
        controller2->put_DefaultBackgroundColor(color); //DO NOT TOUCH!

        // 2. Get the webview handle
        HWND webviewHwnd = SK_Common::getWebview2HWNDForWindow(parentClassName); //DO NOT TOUCH!

        //  3. Bring webview to top
        if (webviewHwnd) { //DO NOT TOUCH!
            SetWindowPos(webviewHwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW); //DO NOT TOUCH!
            SetForegroundWindow(webviewHwnd); //DO NOT TOUCH!
        } //DO NOT TOUCH!

        //  4. Get the current extended styles
        LONG_PTR exStyle = GetWindowLongPtr(webviewHwnd, GWL_EXSTYLE); //DO NOT TOUCH!

        //  5. Modify the extended styles
        exStyle |= WS_EX_LAYERED; //Add layered window support //DO NOT TOUCH!
        SetWindowLongPtr(webviewHwnd, GWL_EXSTYLE, exStyle); //DO NOT TOUCH!

        //  6. Set layered attributes (e.g., transparency)
        SetLayeredWindowAttributes(webviewHwnd, RGB(255, 255, 255), 255, LWA_COLORKEY); //DO NOT TOUCH!

        //  7. Set bounds
        controller->put_Bounds(rect); //DO NOT TOUCH!
    }

	void create() {

        auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
        //options->put_AdditionalBrowserArguments(L"--enable-features=EnableGPUAcceleration");
        //options->put_AdditionalBrowserArguments(L"--disable-software-rasterizer");

        CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, options.Get(),
            Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
                [this](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                    if (FAILED(result)) {
                        // Debug: Log WebView2 environment creation failure
                        return result;
                    }

                    environment = env;

                   

                    // Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
                    env->CreateCoreWebView2Controller(*parentHwnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [this](HRESULT result, ICoreWebView2Controller* _controller) -> HRESULT {
                                                       
                            if (_controller != nullptr) {
                                controller = _controller;
                                controller->get_CoreWebView2(&webview);
                            }



                            controller->add_AcceleratorKeyPressed(Callback<ICoreWebView2AcceleratorKeyPressedEventHandler>([&](ICoreWebView2Controller* sender, ICoreWebView2AcceleratorKeyPressedEventArgs* args) -> HRESULT {
                                COREWEBVIEW2_KEY_EVENT_KIND keyEventKind;
                                args->get_KeyEventKind(&keyEventKind);

                                // Only process key down events
                                if (keyEventKind == COREWEBVIEW2_KEY_EVENT_KIND_KEY_DOWN ||
                                    keyEventKind == COREWEBVIEW2_KEY_EVENT_KIND_SYSTEM_KEY_DOWN) {

                                    UINT key;
                                    args->get_VirtualKey(&key);

                                    // Send the key event to the parent window
                                    PostWindowMessage(WM_KEYDOWN, key, 0);

                                    // Mark the event as handled
                                    args->put_Handled(TRUE);
                                }

                                return S_OK;
                                }).Get(), nullptr);

                            if (webview == nullptr) {
                                return S_OK;
                            }

                            controller->put_IsVisible(true);

                            webview->get_Settings(&settings);
                            settings->put_IsScriptEnabled(TRUE);
                            settings->put_AreDefaultScriptDialogsEnabled(TRUE);
                            settings->put_IsWebMessageEnabled(TRUE);
                            settings->put_AreDefaultContextMenusEnabled(true);
                            settings->put_AreDevToolsEnabled(true);


                            //Configure resource request handling
                            webview->AddWebResourceRequestedFilter(L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);

                            webview->add_WebResourceRequested(Callback<ICoreWebView2WebResourceRequestedEventHandler>([&](ICoreWebView2* sender, ICoreWebView2WebResourceRequestedEventArgs* args) -> HRESULT {

                                SK_Communication_Config config{ "sk.sb", SK_Communication_Packet_Type::sk_comm_pt_web, args, environment };
                                SK_Common::onCommunicationRequest(&config, NULL);

                                return S_OK;
                            }).Get(), nullptr);

                            webview->add_WebMessageReceived(Callback<ICoreWebView2WebMessageReceivedEventHandler>([this](ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
                                wil::unique_cotaskmem_string strPtr;
                                SK_String str;
                                if (SUCCEEDED(args->TryGetWebMessageAsString(&strPtr))) {
                                    str = strPtr.get();
                                }


                                if (str.data.size() > 0 && str.data[0] == '@') {
                                    std::string msg_id;
                                    if (str.data.substr(1, 4) == "none") {
                                        msg_id = str.data.substr(5, str.data.size() - 5);
                                    }
                                    else if (str.data.substr(1, 6) == "yyjson") {
                                        msg_id = str.data.substr(7, str.data.size() - 7);
                                        
                                        yyjson_doc* read_doc = yyjson_read(ipcTestStr.c_str(), ipcTestStr.size(), 0);
                                        if (!read_doc) { throw std::runtime_error("Failed to parse JSON string"); }

                                        yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
                                        yyjson_mut_val* root = yyjson_val_mut_copy(doc, yyjson_doc_get_root(read_doc));
                                        yyjson_mut_doc_set_root(doc, root);
                                       

                                        yyjson_doc_free(read_doc);
                                    }
                                    else if (str.data.substr(1, 8) == "nlohmann") {
                                        msg_id = str.data.substr(9, str.data.size() - 9);

                                        nlohmann::json payload = nlohmann::json::parse(ipcTestStr);
                                    }

                                    SK_String data = "sk_api.ipc.handleIncoming({type: \"ipcTestResponse\", msg_id: \"" + msg_id + "\"})";
                                    evaluateScript(data, NULL);
                                    return S_OK;
                                }


                                wil::unique_cotaskmem_string jsonPStr;
                                SK_String jsonStr;
                                if (SUCCEEDED(args->get_WebMessageAsJson(&jsonPStr))) {
                                    jsonStr = jsonPStr.get();
                                }

                                nlohmann::json payload = nlohmann::json::parse(jsonStr.data);

                                SK_Communication_Config config { "sk.view", SK_Communication_Packet_Type::sk_comm_pt_ipc, &payload };
                                SK_Common::onCommunicationRequest(&config, [&](const SK_String& ipcResponseData) {
                                    SK_String data = "sk_api.ipc.handleIncoming(" + ipcResponseData + ")";
                                    evaluateScript(data, NULL);
                                });


                                return S_OK;
                            }).Get(), &mWebMessageReceivedToken);


                            


                            //----  Lets make the webview transparent  ----//
                            callResize();

                            SK_Common::onWebViewReady(static_cast<void*>(webview.get()), false);

                            //  8. Finally we can navigate to the desired URL
                            //webview->Navigate(L"data:text/html, <html style=\"background:transparent;\"><body style=\"background:transparent; color: white;\">WebView 2</body></html>");

                            navigate(currentURL);

                            return S_OK;
                        }).Get());
                    return S_OK;
                }
            ).Get()
        );

    };

    void PostWindowMessage(_In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
        PostMessage(*parentHwnd, Msg, wParam, lParam);
    };

	void navigate(const SK_String& url){
		currentURL = url;

		if (webview == nullptr) return;

		webview->Navigate(url);
	};

    void evaluateScript_mainThread(wil::com_ptr<ICoreWebView2> webview, const SK_String& src, SK_WebView_EvaluationComplete_Callback cb) {
        LPCWSTR str = src;
        webview->ExecuteScript(str, Callback<ICoreWebView2ExecuteScriptCompletedHandler>([cb](HRESULT err, LPCWSTR resAsWStr) -> HRESULT {
            if (cb != nullptr && resAsWStr) {
                SK_String resAsStr = resAsWStr;
                //nlohmann::json res = nlohmann::json::parse(resAsWStr);
                //cb(res);
            }

            return S_OK;
        }).Get());
    };

    void evaluateScript(const SK_String& src, SK_WebView_EvaluationComplete_Callback cb) {

        if (SK_Thread_Pool::thisFunctionRunningInMainThread()) {
            evaluateScript_mainThread(webview, src, cb);
            return;
        }

        wil::com_ptr<ICoreWebView2> _webview = webview;
        
        SK_Common::threadPool->queueOnMainThread([this, src, cb, _webview]() {
            evaluateScript_mainThread(_webview, src, cb);
        });
    };

    void showDevTools() {
        webview->OpenDevToolsWindow();
    };

private:

};

END_SK_NAMESPACE