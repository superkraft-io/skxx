#pragma once

#include <iostream>
#include <windows.h>
#include <wil/com.h>
#include <wrl.h>
#include <WebView2.h>
#include "WebView2EnvironmentOptions.h"


#pragma comment(lib, "ole32.lib")

#include "../../utils/sk_string.hxx"
#include "sk_webview_windows.hxx"

using namespace Microsoft::WRL;

void SK_WebView::create() {
    CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                if (FAILED(result)) {
                    // Debug: Log WebView2 environment creation failure
                    return result;
                }

                // Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
                env->CreateCoreWebView2Controller(*parentHwnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                    [this](HRESULT result, ICoreWebView2Controller* _controller) -> HRESULT {
                        if (_controller != nullptr)
                        {
                            controller = _controller;
                            controller->get_CoreWebView2(&webview);
                        }

                        if (webview == nullptr)
                        {
                            return S_OK;
                        }


                        webview->get_Settings(&settings);
                        settings->put_IsScriptEnabled(TRUE);
                        settings->put_AreDefaultScriptDialogsEnabled(TRUE);
                        settings->put_IsWebMessageEnabled(TRUE);
                        settings->put_AreDefaultContextMenusEnabled(true);
                        settings->put_AreDevToolsEnabled(true);


                        RECT bounds;
                        GetClientRect(*parentHwnd, &bounds);
                        controller->put_Bounds(bounds);


                        webview->Navigate(L"https://github.com/superkraft-io");

                        navigate(currentURL);

                        return S_OK;
                    }).Get());
                return S_OK;
            }).Get());

}


void SK_WebView::update() {
    if (controller != nullptr) {
        RECT bounds;
        GetClientRect(*parentHwnd, &bounds);
        controller->put_Bounds(bounds);
    };
}




void SK_WebView::navigate(const SK_String& url) {
    currentURL = url;

    if (webview == nullptr) return;

    webview->Navigate(url.toLPCWSTR());
}