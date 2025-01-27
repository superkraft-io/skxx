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