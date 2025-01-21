#pragma once

#include "../../sk_common.hxx"
#include "../../sk_window_mngr/windows/sk_window_windows.hxx"

BEGIN_SK_NAMESPACE

class SK_Window : public SK_Window_Root {
public:

    SK_String windowClassName = "SK_Window";
    HWND hwnd;
    WNDCLASS wc;

    SK_Window::~SK_Window() {
		UnregisterClass(windowClassName.c_str(), wc.hInstance);
	}

    // Window procedure for handling messages
    static LRESULT CALLBACK SK_Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc;
                BeginPaint(hwnd, &ps);

                hdc = GetDC(hwnd);

                EndPaint(hwnd, &ps);

                return true;
            }

            case WM_SIZE: {
                wnd->webview.update();
                return 0;
            }

            case WM_DESTROY: {
                PostQuitMessage(0);
                return 0;
            }

            case WM_CLOSE: {
                DestroyWindow(hwnd);
                return 0;
            }


            case WM_ACTIVATE: {
                if (wParam == WA_INACTIVE)
                {
                    SK_Common::onWindowFocusChanged(wnd, false);
                }
                else
                {
                    SK_Common::onWindowFocusChanged(wnd, true);
                }
                return 0;
            }

            default: {
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }
        };
    }



	void initialize(const unsigned int& _wndIdx) override {
        SK_Window_Root::initialize(_wndIdx);

        windowClassName += "_" + std::to_string(wndIdx);
    };

	void create() {
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        applyTo(hInstance, nullptr, nullptr, 0);
    };

	int applyTo(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
        // Register the window class

        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = windowClassName.c_str();
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);

        if (!RegisterClass(&wc)) {
            MessageBox(nullptr, "Failed to register window class.", "Error", MB_OK | MB_ICONERROR);
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
            windowClassName.c_str(),
            title.c_str(),

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
            MessageBox(nullptr, "Failed to create window.", "Error", MB_OK | MB_ICONERROR);
            return 1;
        }

        // Show the window
        ShowWindow(hwnd, (visible ? SW_SHOW : 0));
        UpdateWindow(hwnd);

        createWebView();

        InvalidateRect(hwnd, nullptr, TRUE);

        SK_Common::updateWebViewHWNDListForView(windowClassName);
    };

	void createWebView() {
        webview.parentHwnd = &hwnd;
        webview.parentClassName = windowClassName;
        webview.create();
        SK_Common::updateWebViewHWNDListForView(windowClassName);
    };

private:

};

END_SK_NAMESPACE