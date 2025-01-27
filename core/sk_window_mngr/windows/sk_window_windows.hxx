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
        static_cast<LONG>(std::ceil((x + w) * scale)),
        static_cast<LONG>(std::ceil((y + h) * scale))
    };
}

class SK_Window : public SK_Window_Root {
public:

    SK_String windowClassName = "SK_Window";
    HWND hwnd, hwndBackground;
    WNDCLASS wc{};
    HINSTANCE hInstance;

    SK_Window() {
        config.onChanged = [&](const std::string& key) {
            if (key == "minWidth" || key == "minHeight" || key == "maxWidth" || key == "maxHeight") {
                this->updateSizeConstraints();
            }
            else if (key == "width" || key == "height") {

            }
            else if (key == "alwaysOnTop") {

            }
            else if (key == "skipTaskbar") {

            }
            else if (key == "noTitle") {

            }
            else if (key == "show") {

            }
            else if (key == "frame") {

            }
            else if (key == "reansparent") {

            }
            else if (key == "backgroundColor") {

            }


        };

    }

    ~SK_Window() {
		UnregisterClass(windowClassName.c_str(), wc.hInstance);
	}



    std::string GetLastErrorAsString() {
        DWORD errorCode = GetLastError();
        if (errorCode == 0) {
            return "No error code available.";
        }

        LPSTR messageBuffer = nullptr;
        size_t size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&messageBuffer,
            0,
            nullptr
        );

        std::string message(messageBuffer, size);
        LocalFree(messageBuffer);
        return message;
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

                if (!wnd->config["transparent"]){
                    PAINTSTRUCT ps;
                    HDC hdc = BeginPaint(hwnd, &ps);

                    HBRUSH hBrush = CreateSolidBrush(RGB(255, 128, 0));

                    RECT clientRect;
                    GetClientRect(hwnd, &clientRect);
                    FillRect(hdc, &clientRect, hBrush);
                    DeleteObject(hBrush);

                    EndPaint(hwnd, &ps);
                }
                else {

                }

                return true;
            }

            case WM_GETMINMAXINFO: {
                // Get the MINMAXconfig structure
                MINMAXINFO* pMinMaxInfo = (MINMAXINFO*)lParam;

                if (wnd){
                    if (wnd->maxSizeFull.x == -1) wnd->maxSizeFull.x = pMinMaxInfo->ptMaxSize.x;
                    if (wnd->maxSizeFull.y == -1) wnd->maxSizeFull.y = pMinMaxInfo->ptMaxSize.y;

                    // Set the maximum size dynamically
                    pMinMaxInfo->ptMinTrackSize.x = wnd->config["minWidth"];
                    pMinMaxInfo->ptMinTrackSize.y = wnd->config["minHeight"];
                    pMinMaxInfo->ptMaxTrackSize.x = (wnd->config["maxWidth"]  > 0 ? wnd->config["maxWidth"]  : wnd->maxSizeFull.x);
                    pMinMaxInfo->ptMaxTrackSize.y = (wnd->config["maxHeight"] > 0 ? wnd->config["maxHeight"] : wnd->maxSizeFull.y);
                }
                return 0;
            }

            case WM_DPICHANGED: {
                WORD dpi = HIWORD(wParam);
                RECT* rect = (RECT*)lParam;
                float scale = getHWNDScale(hwnd);

                POINT diff;
                RECT clientRect, wndRect;

                GetClientRect(hwnd, &clientRect);
                GetWindowRect(hwnd, &wndRect);

                diff.x = (wndRect.right - wndRect.left) - clientRect.right;
                diff.y = (wndRect.bottom - wndRect.top) - clientRect.bottom;


                SetWindowPos(hwnd, 0, rect->left, rect->top, wnd->config["width"] + diff.x, wnd->config["height"] + diff.y, 0);

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
                returnVal = 0;

                wnd->update(true);

                return returnVal;
            }

            case WM_SIZING: {
                wnd->update(true);
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
        hInstance = GetModuleHandle(nullptr);
        applyTo(hInstance, nullptr, nullptr, 0);
    };

	int applyTo(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
        // Register the window class

        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = windowClassName.c_str();
        

        if (!RegisterClass(&wc)) {
            MessageBox(nullptr, "Failed to register window class.", "Error", MB_OK | MB_ICONERROR);
            return 1;
        }

        DWORD wndStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_OVERLAPPEDWINDOW;

        if (config["frame"] == false) {
            wndStyle = WS_POPUP;
        }


        // Create the window
        hwnd = CreateWindowEx(
            0,

            windowClassName.c_str(),
            title.c_str(),

            wndStyle,

            config["left"],
            config["top"],
            config["width"],
            config["height"],

            nullptr,
            nullptr,
            hInstance,
            this
        );

        if (!hwnd) {
            std::string errorMessage = "Failed to create window. Error: " + GetLastErrorAsString();
            MessageBox(nullptr, errorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
        }
      

        if (config["alwaysOnTop"] == true) {
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,SWP_NOMOVE | SWP_NOSIZE);
        }

        float scale = getHWNDScale(hwnd);
        SetWindowPos(hwnd, NULL, 0, 0, config["width"] * scale, config["height"] * scale, SWP_NOMOVE | SWP_NOZORDER);

        // Show the window
        ShowWindow(hwnd, (config["visible"] ? SW_SHOW : 0));
        UpdateWindow(hwnd);

        createWebView();


        SK_Common::updateWebViewHWNDListForView(windowClassName);

        updateSizeConstraints();
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


    void update(bool manuallyResizing = false) {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);


        RECT rect = scaleRect(config["left"], config["top"], clientRect.right, clientRect.bottom, config["scale"]);
        if (webview.webview != nullptr) {
            webview.updateStyling(rect);
            webview.controller->SetBoundsAndZoomFactor(rect, 1);
            //webview.environment->TriggerRepaint();
            RedrawWindow(hwnd, &rect, nullptr, RDW_UPDATENOW | RDW_INVALIDATE | RDW_ALLCHILDREN);
        }
    }



    /********/

    auto hit_test(POINT cursor) const -> LRESULT {
        // identify borders and corners to allow resizing the window.
        // Note: On Windows 10, windows behave differently and
        // allow resizing outside the visible window frame.
        // This implementation does not replicate that behavior.
        const POINT border{
            ::GetSystemMetrics(SM_CXFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER),
            ::GetSystemMetrics(SM_CYFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER)
        };
        RECT window;
        if (!::GetWindowRect(hwnd, &window)) {
            return HTNOWHERE;
        }

        const auto drag = frameless_drag ? HTCAPTION : HTCLIENT;

        enum region_mask {
            client = 0b0000,
            left = 0b0001,
            right = 0b0010,
            top = 0b0100,
            bottom = 0b1000,
        };

        const auto result =
            left * (cursor.x < (window.left + border.x)) |
            right * (cursor.x >= (window.right - border.x)) |
            top * (cursor.y < (window.top + border.y)) |
            bottom * (cursor.y >= (window.bottom - border.y));

        switch (result) {
        case left: return frameless_resize ? HTLEFT : drag;
        case right: return frameless_resize ? HTRIGHT : drag;
        case top: return frameless_resize ? HTTOP : drag;
        case bottom: return frameless_resize ? HTBOTTOM : drag;
        case top | left: return frameless_resize ? HTTOPLEFT : drag;
        case top | right: return frameless_resize ? HTTOPRIGHT : drag;
        case bottom | left: return frameless_resize ? HTBOTTOMLEFT : drag;
        case bottom | right: return frameless_resize ? HTBOTTOMRIGHT : drag;
        case client: return drag;
        default: return HTNOWHERE;
        }
    }

    void updateSizeConstraints() {
        SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
private:

};

END_SK_NAMESPACE