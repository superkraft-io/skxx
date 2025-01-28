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
    HWND hwnd = NULL;
    WNDCLASS wc{};
    HINSTANCE hInstance;

    SK_Window() {
        config.onChanged = [&](const std::string& key) {
            updateWindowByConfig();
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

                    float scale = getHWNDScale(hwnd);

                    // Set the maximum size dynamically
                    pMinMaxInfo->ptMinTrackSize.x = wnd->config["minWidth"] * scale;
                    pMinMaxInfo->ptMinTrackSize.y = wnd->config["minHeight"] * scale;
                    pMinMaxInfo->ptMaxTrackSize.x = (wnd->config["maxWidth"]  > 0 ? wnd->config["maxWidth"] * scale : wnd->maxSizeFull.x);
                    pMinMaxInfo->ptMaxTrackSize.y = (wnd->config["maxHeight"] > 0 ? wnd->config["maxHeight"] * scale : wnd->maxSizeFull.y);
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

            case WM_MOVING: {
                if (wnd->resizing) return 0;

                if (wnd->config.data["movable"] == false){

                    float scale = getHWNDScale(hwnd);

                    // Prevent window from moving
                    int right = wnd->config["x"];
                    int width = wnd->config["width"];
                    right = right + width * scale;

                    int bottom = wnd->config["y"];
                    int height = wnd->config["height"];
                    bottom = bottom + height * scale;

                    RECT* rect = reinterpret_cast<RECT*>(lParam);
                    rect->left = wnd->config["x"];
                    rect->top = wnd->config["y"];
                    rect->right = right;
                    rect->bottom = bottom;
                    
                    return 1; // Indicates we modified the rect
                }

                return 0;
            }

            case WM_MOVE: {
                if (wnd->resizing) return 0;

                if (wnd->config["movable"] == false) {
                    SetWindowPos(hwnd, NULL, wnd->config["x"], wnd->config["y"], 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                    return 1;
                }

                //int x = LOWORD(lParam);
                //int y = HIWORD(lParam);
                RECT wndRect;
                GetWindowRect(hwnd, &wndRect);
                wnd->config.data["x"] = wndRect.left;
                wnd->config.data["y"] = wndRect.top;

                return 0;
            }


            case WM_SIZE: {
                switch (LOWORD(wParam)) {
                    case SIZE_RESTORED: {
                        int x = 0;
                    }

                    case SIZE_MAXIMIZED: {
                        /*
                        
                            OPTION 1
                            set a flag to indicate that size has maximized
                            use the flag inside updateWindowByConfig() to prevent size from being manipulated (maybe use SWP_NOSIZE or something?)
                            (this option is probably best)
                         
                            OPTION 2
                            get w and h
                            check if maxWidth and maxHeight are set to above 0 in config
                            if they are, clamp w and h to their maxes
                            if they are not, apply screen size w and h in config
                        */
                        int y = 0;
                    }
                }


                wnd->update(true);

                return 0;
            }

            case WM_SIZING: {
                wnd->resizing = true;

                RECT* rect = reinterpret_cast<RECT*>(lParam);

                // Extract x, y, width, and height
                int x = rect->left;
                int y = rect->top;
                int width = rect->right - rect->left;
                int height = rect->bottom - rect->top;

                float scale = getHWNDScale(hwnd);

                float floatScaledWidth = width / scale;
                float floatScaledHeight = height / scale;
                int scaledWidth = (int)floatScaledWidth;
                int scaledHeight = (int)floatScaledHeight;

                wnd->config.data["x"] = x;
                wnd->config.data["y"] = y;
                wnd->config.data["width"] = scaledWidth;
                wnd->config.data["height"] = scaledHeight;

                wnd->update(true);
                return 0;
            }

            case WM_EXITSIZEMOVE: {
                wnd->resizing = false;
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

            config["x"],
            config["y"],
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

        updateWindowByConfig();
        UpdateWindow(hwnd);
        createWebView();

        SK_Common::updateWebViewHWNDListForView(windowClassName);


    };

	void createWebView() {
        webview.callResize = [&]() { update(); };

        webview.parentHwnd = &hwnd;
        webview.parentClassName = windowClassName;
        webview.create();
        SK_Common::updateWebViewHWNDListForView(windowClassName);
    };


    void update(bool manuallyResizing = false) {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);


        RECT rect = scaleRect(config["x"], config["y"], clientRect.right, clientRect.bottom, config["scale"]);
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

    void setStyle(DWORD style, bool activate) {
        if (activate) SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | style);
        else SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~style);
    }

    void updateWindowByConfig() {
        if (hwnd == NULL) return;

        float scale = getHWNDScale(hwnd);

        //movable: handled in WindowProc
        if (checkNeedsUpdateAndReset("resizable")) setStyle(WS_SIZEBOX, config.data["resizable"]);
        if (checkNeedsUpdateAndReset("alwaysOnTop")) setAlwaysOnTop(config.data["alwaysOnTop"]);
        if (checkNeedsUpdateAndReset("maximizable")) setStyle(WS_MAXIMIZEBOX, config.data["maximizable"]);
        if (checkNeedsUpdateAndReset("minimizable")) setStyle(WS_MINIMIZEBOX, config.data["minimizable"]);

        if (checkNeedsUpdateAndReset("closable")) {
            HMENU hMenu = GetSystemMenu(hwnd, FALSE);
            if (hMenu) {
                if (config.data["closable"] == false) EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
                else EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);
                DrawMenuBar(hwnd);
            }
        }



        if (checkNeedsUpdateAndReset("center") && config.data["center"] == true) {
            RECT  wndRect;
            GetWindowRect(hwnd, &wndRect);

            int wndWidth  = (wndRect.right - wndRect.left);
            int wndHeight = (wndRect.bottom - wndRect.top);

            if (config.data["minWidth"] > 0 && wndWidth > config.data["minWidth"]) wndWidth = config.data["minWidth"];
            if (config.data["minHeight"] > 0 && wndHeight > config.data["minHeight"]) wndHeight = config.data["minHeight"];

            if (config.data["maxWidth"] > 0 && wndWidth > config.data["maxWidth"]) wndWidth = config.data["maxWidth"];
            if (config.data["maxHeight"] > 0 && wndHeight > config.data["maxHeight"]) wndHeight = config.data["maxHeight"];

            wndWidth *= scale;
            wndHeight *= scale;

            int posx = GetSystemMetrics(SM_CXSCREEN) / 2 - wndWidth / 2;
            int posy = GetSystemMetrics(SM_CYSCREEN) / 2 - wndHeight / 2;

            config.data["x"] = posx;
            config.data["y"] = posy;
        }


        //everything below this comment should come last
        if (checkNeedsUpdateAndReset("x") || checkNeedsUpdateAndReset("y") || checkNeedsUpdateAndReset("width") || checkNeedsUpdateAndReset("width")) SetWindowPos(hwnd, NULL, config.data["x"], config.data["y"], config["width"] * scale, config["height"] * scale, SWP_NOZORDER);
        if (checkNeedsUpdateAndReset("show")) ShowWindow(hwnd, (config["show"] ? SW_SHOW : SW_HIDE));
    }

    void setAlwaysOnTop(bool flag, int level = 0, int relativeLevel = 0) {
        config.data["alwaysOnTop"] = flag;
        if (flag == true)  SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        else SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
private:

};

END_SK_NAMESPACE