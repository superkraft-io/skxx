#pragma once

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>

#include "../../utils/sk_str_utils.hxx"

#include "sk_window_windows.h"

#if defined(SK_FRAMEWORK_iPlug2)
#include "../../sk_window_mngr/root/sk_window_root.hxx"
#else
#include "../root/sk_window_root.h"
#endif


SK_Window::~SK_Window() {
    UnregisterClass(windowClassName.toLPCWSTR(), wc.hInstance);
}

// Window procedure for handling messages
LRESULT CALLBACK SK_Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    SK_Window* wnd;
        
    if (uMsg == WM_NCCREATE) {
        //Set the owner of this window upon creation
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        wnd = static_cast<SK_Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(wnd));
    }
    else {
        //Retrieve the owner so that other messages can utilize the window
        wnd = reinterpret_cast<SK_Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    

    switch (uMsg) {
        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hdc;
            BeginPaint(hwnd, &ps);

            hdc = GetDC(hwnd);

            EndPaint(hwnd, &ps);
            return true;
    

        case WM_SIZE:
            wnd->webview.update();
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    };
}


 void SK_Window::initialize(const unsigned int& _wndIdx) {
     SK_Window_Root::initialize(_wndIdx);

     windowClassName += "_" + std::to_string(wndIdx);
 }
 
 void SK_Window::create() {
     HINSTANCE hInstance = GetModuleHandle(nullptr);
     applyTo(hInstance, nullptr, nullptr, 0);
 }

int SK_Window::applyTo(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
    // Register the window class
    
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = windowClassName.toLPCWSTR();
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc)) {
        MessageBox(nullptr, L"Failed to register window class.", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    DWORD wndStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_OVERLAPPEDWINDOW;

    RECT rect;
    rect.left = left;
    rect.top = top;
    rect.right = left + width;
    rect.bottom = top + height;
    AdjustWindowRect(&rect, wndStyle, false);

    // Create the window
    hwnd = CreateWindowEx(
        0,
        windowClassName.toLPCWSTR(),
        title.toLPCWSTR(),
        
        wndStyle,
        
        left,
        top,
        rect.right - rect.left,
        rect.bottom - rect.top,

        nullptr,
        nullptr,
        hInstance,
        this
    );

    if (!hwnd) {
        MessageBox(nullptr, L"Failed to create window.", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Initialize WebView2
    /*if (FAILED(InitializeWebView(hwnd))) {
        MessageBox(hwnd, L"Failed to initialize WebView2.", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }*/

    // Show the window
    ShowWindow(hwnd, (visible ? SW_SHOW : 0));
    UpdateWindow(hwnd);

    createWebView();

    InvalidateRect(hwnd, nullptr, TRUE);

}

void SK_Window::createWebView() {
    webview.parentHwnd = &hwnd;
    webview.create();
}