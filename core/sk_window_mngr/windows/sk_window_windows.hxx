#pragma once

#include "../../sk_common.hxx"

BEGIN_SK_NAMESPACE


inline UINT(WINAPI* sk__GetDpiForWindow)(HWND);

static inline float getHWNDScale(HWND hwnd)
{
    if (!sk__GetDpiForWindow) {
        HINSTANCE h = LoadLibraryW(L"user32.dll");
        if (h) *(void**)&sk__GetDpiForWindow = GetProcAddress(h, "GetDpiForWindow");

        if (!sk__GetDpiForWindow) return 1;
    }

    int dpi = sk__GetDpiForWindow(hwnd);

    if (dpi != USER_DEFAULT_SCREEN_DPI) {
        return static_cast<float>(dpi) / USER_DEFAULT_SCREEN_DPI;
    }

    return 1;
}


static inline RECT scaleRect(float x, float y, float w, float h, float scale) {
    return {
        static_cast<LONG>(std::ceil(x * scale)),
        static_cast<LONG>(std::ceil(y * scale)),
        static_cast<LONG>(std::ceil((x + w) * scale)) + 1,
        static_cast<LONG>(std::ceil((y + h) * scale)) + 1
    };
}

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


            case WM_DPICHANGED:
            {
                WORD dpi = HIWORD(wParam);
                RECT* rect = (RECT*)lParam;
                float scale = getHWNDScale(hwnd);

                POINT diff;
                RECT clientRect, wndRect;

                GetClientRect(hwnd, &clientRect);
                GetWindowRect(hwnd, &wndRect);

                diff.x = (wndRect.right - wndRect.left) - clientRect.right;
                diff.y = (wndRect.bottom - wndRect.top) - clientRect.bottom;


                SetWindowPos(hwnd, 0, rect->left, rect->top, wnd->width + diff.x, wnd->height + diff.y, 0);

                return 0;
            }
            case WM_SIZE: {
                int returnVal = 0;
                switch (LOWORD(wParam)) {
                    case SIZE_RESTORED:
                    case SIZE_MAXIMIZED: {
                        returnVal = 1;
                    }
                    default:
                        returnVal = 0;
                }

                RECT r;
                GetClientRect(hwnd, &r);
                wnd->left = r.left;
                wnd->top = r.top;
                wnd->width = r.right - r.left;
                wnd->height = r.bottom - r.top;

                wnd->webview.update();

                return returnVal;
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
                if (wParam == WA_INACTIVE) {
                    SK_Common::onWindowFocusChanged(wnd, false);
                }
                else {
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
        webview.callResize = [&]() {
            update();
        };
        webview.parentHwnd = &hwnd;
        webview.parentClassName = windowClassName;
        webview.create();
        SK_Common::updateWebViewHWNDListForView(windowClassName);
    };


    void update() {
        RECT rect = scaleRect(left, top, width, height, getHWNDScale(hwnd));
        if (webview.webview != nullptr) {
            webview.updateStyling(rect);
            webview.controller->SetBoundsAndZoomFactor(rect, scale);
        }
    }
private:

};

END_SK_NAMESPACE