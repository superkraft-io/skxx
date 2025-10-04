#pragma once

#include "../../sk_common.hpp"

BEGIN_SK_NAMESPACE


static RECT scaleRect(float x, float y, float w, float h, float scale) {
    return {
        static_cast<LONG>(std::ceil(x * scale)),
        static_cast<LONG>(std::ceil(y * scale)),
        static_cast<LONG>(std::ceil((x + w) * scale)),
        static_cast<LONG>(std::ceil((y + h) * scale))
    };
}

using SK_Window_WndEvent_CB = std::function<void(const SK_String& eventID, nlohmann::json data)>;

class SK_Window : public SK_Window_Root {
public:

    bool ignoreUpdateByConfig = false;

    UINT(WINAPI* sk__GetDpiForWindow)(HWND) = nullptr;

    float getHWNDScale(HWND hwnd)
    {
        if (!sk__GetDpiForWindow) {
            HINSTANCE h = LoadLibraryW(L"user32.dll");
            if (h) *(void**)&sk__GetDpiForWindow = GetProcAddress(h, "GetDpiForWindow");

            if (!sk__GetDpiForWindow) return 1;
        }

        if (hwnd) {
            int dpi = sk__GetDpiForWindow(hwnd);

            if (dpi != USER_DEFAULT_SCREEN_DPI) {
                return static_cast<float>(dpi) / USER_DEFAULT_SCREEN_DPI;
            }
        }

        return 1;
    }



    SK_String windowClassName = "SK_Window_" + skg->newUUID();
    HWND wndHandle = NULL;
    WNDCLASSW wc = { 0 };
    HINSTANCE hInstance;

    bool sysCtxMenuTriggered = false;
    bool shouldPreventSysCtxMenu = false;
    bool shouldPreventSysCtxMenu_2ndPass = false;
    SK_Point sysCtxMenuPos;


    SK_Window() {
        config.onChanged = [&](const std::string& key) {
            config_updateTracker[key] = true;
            updateWindowByConfig();
        };

        ipc->on("isReady", [&](const nlohmann::json& data, SK_Communication_Packet* packet) {
            packet->response()->JSON({ {"isReady", isReady} });
        });

        onWindowAction = [&](SK_Communication_Packet* packet) {
            //do something here
        };

        ipc->onMessage = [&, this](const SK_String& sender, SK_Communication_Packet* packet) {
            SK_String action = "";
            if (packet->data.contains("action")) action = SK_String(packet->data["action"]);
            if (action == "startDraggingWindow") {
                ReleaseCapture();

                // Use current cursor position (screen coords)
                POINT pt;
                GetCursorPos(&pt);

                // Tell the window �the user pressed down on the title bar here�
                SendMessage(wndHandle, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(pt.x, pt.y));
            }
        };
    }

    ~SK_Window() {
		UnregisterClassW(windowClassName.toWString().c_str(), wc.hInstance);
		delete ipc;
	}



    int handleWndEvents(SK_Window* wnd, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        int returnVal = -1;

        switch (msg) {
            case WM_QUIT: {
                int y = 0;
                break;
            }

            case WM_ENTERSIZEMOVE:
                break;

            case WM_MOVING:
                if (!wnd->isMoving) {
                    wnd->isMoving = true;
                    emitWndEvent(wnd, "will-move", {});
                }

                emitWndEvent(wnd, "move", {});
                emitWndEvent(wnd, "moved", {});
                break;


            case WM_MOVE:
                break;



            case WM_SIZING: {
                SK_String edge = "none";

                switch (wParam) {
                case WMSZ_LEFT:
                    edge = "left";
                    break;
                case WMSZ_RIGHT:
                    edge = "right";
                    break;
                case WMSZ_TOP:
                    edge = "top";
                    break;
                case WMSZ_BOTTOM:
                    edge = "bottom";
                    break;
                case WMSZ_TOPLEFT:
                    edge = "top-left";
                    break;
                case WMSZ_TOPRIGHT:
                    edge = "rop-right";
                    break;
                case WMSZ_BOTTOMLEFT:
                    edge = "bottom-left";
                    break;
                case WMSZ_BOTTOMRIGHT:
                    edge = "bottom-right";
                    break;
                }


                LPRECT pRect = (LPRECT)lParam;

                if (!wnd->isResizing) {
                    wnd->isResizing = true;
                    emitWndEvent(wnd, "will-resize", {
                        {"newBounds", {
                            {"x", pRect->left},
                            {"y", pRect->top},
                            {"width", pRect->right - pRect->left},
                            {"height", pRect->bottom - pRect->top},
                        }},
                        {"edge", edge}
                        });
                }

                switch (wParam) {
                case SIZE_MAXIMIZED:
                    emitWndEvent(wnd, "maximize", {});
                    break;
                case SIZE_MINIMIZED:
                    emitWndEvent(wnd, "minimize", {});
                    break;
                case SIZE_RESTORED:
                    if (lParam != 0) emitWndEvent(wnd, "resized", {});
                    else emitWndEvent(wnd, "restore", {});
                    break;
                }

                emitWndEvent(wnd, "resize", {});
                break;
            }

            case WM_EXITSIZEMOVE:
                if (wnd->isMoving) emitWndEvent(wnd, "move-end", {});
                if (wnd->isResizing) emitWndEvent(wnd, "resize-end", {});

                wnd->isMoving = false;
                wnd->isResizing = false;
                break;

            case WM_SYSCOMMAND:
                if (wParam == SC_MAXIMIZE) {
                    wnd->isMaximized = true;
                    wnd->isMinimized = false;
                    emitWndEvent(wnd, "maximize", {});
                }
                else if (wParam == SC_MINIMIZE) {
                    wnd->isMaximized = false;
                    wnd->isMinimized = true;
                    emitWndEvent(wnd, "minimize", {});
                }
                else if (wParam == SC_RESTORE) {
                    if (wnd->isMaximized) emitWndEvent(wnd, "unmaximize", {});
                    if (wnd->isMinimized) emitWndEvent(wnd, "restore", {});
                    wnd->isMaximized = false;
                    wnd->isMinimized = false;
                }
                break;



            case WM_NCRBUTTONDOWN:
                if (wParam == HTSYSMENU || wParam == HTCAPTION) {
                    wnd->sysCtxMenuTriggered = true;
                }
                break;


            case WM_CLOSE:
                if (!wnd->shouldClose_2ndPass) {
                    emitWndEvent(wnd, "close", {}, [wnd](nlohmann::json response) {
                        if (response.contains("defaultPrevented") && response["defaultPrevented"] == true) {
                            wnd->shouldClose = false;
                        }

                        wnd->shouldClose_2ndPass = true;
                        PostMessage(wnd->wndHandle, WM_CLOSE, 0, 0);
                    });
                } else {
                    if (wnd->shouldClose) {
                        DestroyWindow(wnd->wndHandle);
                        emitWndEvent(wnd, "closed", {});
                    }
                }

                wnd->shouldClose_2ndPass = false;
                wnd->shouldClose = true;

                break;

            case WM_DESTROY:
                emitWndEvent(wnd, "closed", {});
                break;

            case WM_KILLFOCUS:
                emitWndEvent(wnd, "blur", {});
                break;

            case WM_SETFOCUS:
                emitWndEvent(wnd, "focus", {});
                break;

            case WM_SHOWWINDOW:
                //if (wParam) emitEvent("show", {});
                //else emitEvent("hide", {});
                break;





            case WM_GETMINMAXINFO:

                break;

            case WM_DISPLAYCHANGE:
                //emitWndEvent(wnd, "enter-full-screen", {});
                break;

            case WM_WINDOWPOSCHANGED:
                if (((WINDOWPOS*)lParam)->flags & SWP_FRAMECHANGED) {
                    //if (IsZoomed(hwnd)) emitWndEvent(wnd, "enter-full-screen", {});
                    //else emitWndEvent(wnd, "leave-full-screen", {});
                }
                break;

            case WM_ENDSESSION:
                emitWndEvent(wnd, "session-end", {});
                break;

            case WM_WINDOWPOSCHANGING:
                if (((WINDOWPOS*)lParam)->flags & SWP_NOZORDER) {
                    //emitWndEvent(wnd, "always-on-top-changed", {});
                }
                break;

            case WM_APPCOMMAND: {
                int cmdRes = GET_APPCOMMAND_LPARAM(lParam);

                SK_String cmd = "unknown";

                switch (cmdRes) {
                    case APPCOMMAND_BROWSER_BACKWARD: cmd = "BROWSER_BACKWARD"; break;
                    case APPCOMMAND_BROWSER_FORWARD: cmd = "BROWSER_FORWARD"; break;
                    case APPCOMMAND_BROWSER_REFRESH: cmd = "BROWSER_REFRESH"; break;
                    case APPCOMMAND_BROWSER_STOP: cmd = "BROWSER_STOP"; break;
                    case APPCOMMAND_BROWSER_SEARCH: cmd = "BROWSER_SEARCH"; break;
                    case APPCOMMAND_BROWSER_FAVORITES: cmd = "BROWSER_FAVORITES"; break;
                    case APPCOMMAND_BROWSER_HOME: cmd = "BROWSER_HOME"; break;
                    case APPCOMMAND_VOLUME_MUTE: cmd = "VOLUME_MUTE"; break;
                    case APPCOMMAND_VOLUME_DOWN: cmd = "VOLUME_DOWN"; break;
                    case APPCOMMAND_VOLUME_UP: cmd = "VOLUME_UP"; break;
                    case APPCOMMAND_MEDIA_NEXTTRACK: cmd = "MEDIA_NEXTTRACK"; break;
                    case APPCOMMAND_MEDIA_PREVIOUSTRACK: cmd = "MEDIA_PREVIOUSTRACK"; break;
                    case APPCOMMAND_MEDIA_STOP: cmd = "MEDIA_STOP"; break;
                    case APPCOMMAND_MEDIA_PLAY_PAUSE: cmd = "MEDIA_PLAY_PAUSE"; break;
                    case APPCOMMAND_LAUNCH_MAIL: cmd = "LAUNCH_MAIL"; break;
                    case APPCOMMAND_LAUNCH_MEDIA_SELECT: cmd = "LAUNCH_MEDIA_SELECT"; break;
                    case APPCOMMAND_LAUNCH_APP1: cmd = "LAUNCH_APP1"; break;
                    case APPCOMMAND_LAUNCH_APP2: cmd = "LAUNCH_APP2"; break;
                    case APPCOMMAND_BASS_DOWN: cmd = "BASS_DOWN"; break;
                    case APPCOMMAND_BASS_BOOST: cmd = "BASS_BOOST"; break;
                    case APPCOMMAND_BASS_UP: cmd = "BASS_UP"; break;
                    case APPCOMMAND_TREBLE_DOWN: cmd = "TREBLE_DOWN"; break;
                    case APPCOMMAND_TREBLE_UP: cmd = "TREBLE_UP"; break;
                    case APPCOMMAND_MICROPHONE_VOLUME_MUTE: cmd = "MICROPHONE_VOLUME_MUTE"; break;
                    case APPCOMMAND_MICROPHONE_VOLUME_DOWN: cmd = "MICROPHONE_VOLUME_DOWN"; break;
                    case APPCOMMAND_MICROPHONE_VOLUME_UP: cmd = "MICROPHONE_VOLUME_UP"; break;
                    case APPCOMMAND_HELP: cmd = "HELP"; break;
                    case APPCOMMAND_FIND: cmd = "FIND"; break;
                    case APPCOMMAND_NEW: cmd = "NEW"; break;
                    case APPCOMMAND_OPEN: cmd = "OPEN"; break;
                    case APPCOMMAND_CLOSE: cmd = "CLOSE"; break;
                    case APPCOMMAND_SAVE: cmd = "SAVE"; break;
                    case APPCOMMAND_PRINT: cmd = "PRINT"; break;
                    case APPCOMMAND_UNDO: cmd = "UNDO"; break;
                    case APPCOMMAND_REDO: cmd = "REDO"; break;
                    case APPCOMMAND_COPY: cmd = "COPY"; break;
                    case APPCOMMAND_CUT: cmd = "CUT"; break;
                    case APPCOMMAND_PASTE: cmd = "PASTE"; break;
                    case APPCOMMAND_REPLY_TO_MAIL: cmd = "REPLY_TO_MAIL"; break;
                    case APPCOMMAND_FORWARD_MAIL: cmd = "FORWARD_MAIL"; break;
                    case APPCOMMAND_SEND_MAIL: cmd = "SEND_MAIL"; break;
                    case APPCOMMAND_SPELL_CHECK: cmd = "SPELL_CHECK"; break;
                    case APPCOMMAND_DICTATE_OR_COMMAND_CONTROL_TOGGLE: cmd = "DICTATE_OR_COMMAND_CONTROL_TOGGLE"; break;
                    case APPCOMMAND_MIC_ON_OFF_TOGGLE: cmd = "MIC_ON_OFF_TOGGLE"; break;
                    case APPCOMMAND_CORRECTION_LIST: cmd = "CORRECTION_LIST"; break;
                    case APPCOMMAND_MEDIA_PLAY: cmd = "MEDIA_PLAY"; break;
                    case APPCOMMAND_MEDIA_PAUSE: cmd = "MEDIA_PAUSE"; break;
                    case APPCOMMAND_MEDIA_RECORD: cmd = "MEDIA_RECORD"; break;
                    case APPCOMMAND_MEDIA_FAST_FORWARD: cmd = "MEDIA_FAST_FORWARD"; break;
                    case APPCOMMAND_MEDIA_REWIND: cmd = "MEDIA_REWIND"; break;
                    case APPCOMMAND_MEDIA_CHANNEL_UP: cmd = "MEDIA_CHANNEL_UP"; break;
                    case APPCOMMAND_MEDIA_CHANNEL_DOWN: cmd = "MEDIA_CHANNEL_DOWN"; break;
                    case APPCOMMAND_DELETE: cmd = "DELETE"; break;
                    case APPCOMMAND_DWM_FLIP3D: cmd = "DWM_FLIP3D"; break;
                    default: cmd = "unknown"; break;
                }

                emitWndEvent(wnd, "app-command", { {"command", cmd.toLowerCase()}});
            }

            case WM_NCHITTEST: {
                if (wnd->activateMoving) {
                    POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                    ScreenToClient(hwnd, &pt);

                    return HTCAPTION;
                }
            }
        }







        if (wnd->sysCtxMenuTriggered) {
            if (wParam == HTSYSMENU || wParam == HTCAPTION) {
                // Extract the X and Y coordinates from lParam
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);


                if (!wnd->shouldPreventSysCtxMenu_2ndPass && !wnd->shouldPreventSysCtxMenu) {
                    wnd->sysCtxMenuPos.x = x;
                    wnd->sysCtxMenuPos.y = y;

                    emitWndEvent(wnd, "system-context-menu",
                        {
                            {"point", {
                                {"x", x},
                                {"y", y}
                            }}
                        },

                        [wnd, x, y](nlohmann::json response) {
                            if (response.contains("defaultPrevented") && response["defaultPrevented"] == true) {
                                wnd->shouldPreventSysCtxMenu = true;
                            }
                            else {
                                wnd->shouldPreventSysCtxMenu = false;
                            }

                            wnd->shouldPreventSysCtxMenu_2ndPass = true;
                            SendMessage(wnd->wndHandle, WM_NCRBUTTONDOWN, HTCAPTION, 0);
                        }
                    );
                }
            }

            wnd->sysCtxMenuTriggered = false;

            if (wnd->shouldPreventSysCtxMenu_2ndPass) {
                wnd->shouldPreventSysCtxMenu_2ndPass = false;
                if (wnd->shouldPreventSysCtxMenu) {
                    wnd->shouldPreventSysCtxMenu = false;
                    return 0;
                }

                // Get the system menu handle
                HMENU hSystemMenu = GetSystemMenu(hwnd, FALSE);

                // Display the system menu at the specified position
                TrackPopupMenu(
                    hSystemMenu,            // Handle to the system menu
                    TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, // Flags
                    wnd->sysCtxMenuPos.x,                      // X-coordinate (screen coordinates)
                    wnd->sysCtxMenuPos.y,                      // Y-coordinate (screen coordinates)
                    0,                      // Reserved (must be 0)
                    hwnd,                   // Handle to the owner window
                    nullptr                 // Ignored
                );
            }
            else {
                wnd->shouldPreventSysCtxMenu_2ndPass = false;
                return 0;
            }
        }


        return returnVal;
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

        if (wnd) {
            int wndEventReturnVal = wnd->handleWndEvents(wnd, hwnd, uMsg, wParam, lParam);
            if (wndEventReturnVal > -1) return wndEventReturnVal;
        }

        switch (uMsg) {
            case WM_KEYDOWN: {
                if (!wnd) return 0;

                if (!wnd->config.data["kiosk"] && wnd->config.data["fullscreenable"]){
                    if (wParam == VK_F11) {
                        wnd->config["fullscreen"] = !wnd->config.data["fullscreen"];
                        return 0;
                    }

                    /*
                    //ESC key not used in ElectronJS to exit fullscreen in windows
                    if (wParam == VK_ESCAPE && wnd) {
                        if (wnd->config.data["fullscreen"]) wnd->config["fullscreen"] = false;
                        return 0;
                    }*/
                }
                break;
            }


            case WM_PAINT: {

                if (!wnd->config.data["transparent"]){
                    PAINTSTRUCT ps;
                    HDC hdc = BeginPaint(hwnd, &ps);

                    HBRUSH hBrush = CreateSolidBrush(RGB(wnd->backgroundColor.r, wnd->backgroundColor.g, wnd->backgroundColor.b));

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
                    if (wnd->config.data["fullscreen"]) return 0;

                    if (wnd->maxSizeFull.x == -1) wnd->maxSizeFull.x = pMinMaxInfo->ptMaxSize.x;
                    if (wnd->maxSizeFull.y == -1) wnd->maxSizeFull.y = pMinMaxInfo->ptMaxSize.y;

                    float scale = wnd->getHWNDScale(hwnd);

                    // Set the maximum size dynamically
                    pMinMaxInfo->ptMinTrackSize.x = float(wnd->config.data["minWidth"]) * scale;
                    pMinMaxInfo->ptMinTrackSize.y = float(wnd->config.data["minHeight"]) * scale;
                    pMinMaxInfo->ptMaxTrackSize.x = (wnd->config.data["maxWidth"]  > 0 ? float(wnd->config.data["maxWidth"]) * scale : float(wnd->maxSizeFull.x));
                    pMinMaxInfo->ptMaxTrackSize.y = (wnd->config.data["maxHeight"] > 0 ? float(wnd->config.data["maxHeight"]) * scale : float(wnd->maxSizeFull.y));
                }
                return 0;
            }

            case WM_DPICHANGED: {
                WORD dpi = HIWORD(wParam);
                RECT* rect = (RECT*)lParam;
                float scale = wnd->getHWNDScale(hwnd);

                POINT diff;
                RECT clientRect, wndRect;

                GetClientRect(hwnd, &clientRect);
                GetWindowRect(hwnd, &wndRect);

                diff.x = (wndRect.right - wndRect.left) - clientRect.right;
                diff.y = (wndRect.bottom - wndRect.top) - clientRect.bottom;


                SetWindowPos(hwnd, 0, rect->left, rect->top, long(wnd->config.data["width"]) + diff.x, long(wnd->config.data["height"]) + diff.y, 0);

                return 0;
            }

            

            case WM_DESTROY: {
                return 0;
            }

            case WM_CLOSE: {
                return 0;
            }


            case WM_ACTIVATE: {
                if (wParam == WA_INACTIVE) {
                    wnd->skg->onWindowFocusChanged(wnd, false);
                }
                else {
                    wnd->skg->onWindowFocusChanged(wnd, true);
                }

                return 0;
            }

            case WM_MOVING: {
                if (wnd->config.data["fullscreen"]) return 0;
                if (wnd->isMaximized) return 0;
                if (wnd->resizing) return 0;

                if (wnd->config.data["movable"] == false){

                    float scale = wnd->getHWNDScale(hwnd);

                    // Prevent window from moving
                    int right = wnd->config.data["x"];
                    int width = wnd->config.data["width"];
                    right = right + width * scale;

                    int bottom = wnd->config.data["y"];
                    int height = wnd->config.data["height"];
                    bottom = bottom + height * scale;

                    RECT* rect = reinterpret_cast<RECT*>(lParam);
                    rect->left = wnd->config.data["x"];
                    rect->top = wnd->config.data["y"];
                    rect->right = right;
                    rect->bottom = bottom;
                    
                    return 1; // Indicates we modified the rect
                }

                return 0;
            }

            case WM_MOVE: {
                if (wnd->config.data["fullscreen"]) return 0;

                WINDOWPLACEMENT wp;
                wp.length = sizeof(WINDOWPLACEMENT);
                if (GetWindowPlacement(hwnd, &wp)) {
                    if (wp.showCmd == SW_MAXIMIZE) {
                        wnd->isMaximized = true;
                        //if (wnd->config.data["fullscreenable"]) wnd->setFullscreen(true);
                    }
                    else {
                        wnd->isMaximized = false;
                        //if (wnd->config.data["fullscreenable"]) wnd->setFullscreen(false);
                    }
                }

                if (wnd->isMaximized) return 0;
                if (wnd->resizing) return 0;

                if (wnd->config.data["movable"] == false) {
                    SetWindowPos(hwnd, NULL, wnd->config.data["x"], wnd->config.data["y"], 0, 0, SWP_NOSIZE | SWP_NOZORDER);
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
                if (wnd->config.data["fullscreen"]) return 0;

                switch (LOWORD(wParam)) {
                    case SIZE_RESTORED: {
                        wnd->isMaximized = false;
                        break;
                    }

                    case SIZE_MAXIMIZED: {
                        wnd->isMaximized = true;
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
                        break;
                    }
                }


                wnd->update(true);

                return 0;
            }

            case WM_SIZING: {
                if (wnd->isMaximized) return 0;

                wnd->resizing = true;

                RECT* rect = reinterpret_cast<RECT*>(lParam);

                // Extract x, y, width, and height
                int x = rect->left;
                int y = rect->top;
                int width = rect->right - rect->left;
                int height = rect->bottom - rect->top;

                float scale = wnd->getHWNDScale(hwnd);

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
                if (wnd->config.data["fullscreen"]) return 0;

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

        std::wstring classNameWStr = windowClassName.toWString();
        wc.lpszClassName = classNameWStr.c_str();
        

        if (!RegisterClassW(&wc)) {
            DWORD error = GetLastError();

            wchar_t errorMsg[256];
            swprintf(errorMsg, 256, L"Failed to register window class. Error code: %lu", error);

            MessageBoxW(nullptr, errorMsg, L"Error", MB_OK | MB_ICONERROR);
            return 1;
        }

        DWORD wndStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_OVERLAPPEDWINDOW;

        

        // Create the window
        wndHandle = CreateWindowExW(
            0,

            classNameWStr.c_str(),
            SK_String(config["title"]).toWString().c_str(),

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

        if (!wndHandle) {
            SK_String errorMessage = "Failed to create window. Error: " + GetLastErrorAsString();
            MessageBoxW(nullptr, errorMessage.toWString().c_str(), L"Error", MB_OK | MB_ICONERROR);
        }

        updateWindowByConfig();
        UpdateWindow(wndHandle);
        createWebView();

        skg->updateWebViewHWNDListForView(windowClassName);


    };

	void createWebView(SK_wndCreated cb = NULL) {
        webview.get_isReady = [&]() { return isReady; };
        webview.callResize = [&]() { update(); };
        webview.notifyReadyToShow = [this, cb]() {
            isReady = true;
            emitWndEvent(this, "ready-to-show", {});
            if (cb) cb(this);
        };

        webview.onGetUserDataPath = [this](SK_Window* _null_) {
            if (skg->onGetWebViewUserDataPath) return skg->onGetWebViewUserDataPath(this);
            return SK_String("");
        };

        webview.parentHwnd = &wndHandle;
        webview.parentClassName = windowClassName;
        webview.create();
        skg->updateWebViewHWNDListForView(windowClassName);
    };


    void update(bool manuallyResizing = false) {
        RECT clientRect;
        GetClientRect(wndHandle, &clientRect);

        int x = 0;
        int y = 0;

        if (config.data["mainWindow"]) {
            x = config.data["x"];
            y = config.data["y"];
        }

        RECT rect = scaleRect(x, y, clientRect.right, clientRect.bottom, config.data["scale"]);
        if (webview.webview != nullptr) {
            webview.updateStyling(rect);
            updateWebView();
            RedrawWindow(wndHandle, &rect, nullptr, RDW_UPDATENOW | RDW_INVALIDATE | RDW_ALLCHILDREN);
        }
    }

    void updateWebView() {
        if (webview.webview == nullptr) return;
        RECT clientRect;
        GetClientRect(wndHandle, &clientRect);
        RECT rect = scaleRect(0, 0, clientRect.right, clientRect.bottom, 1);
        webview.controller->SetBoundsAndZoomFactor(rect, 1);
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
        if (!::GetWindowRect(wndHandle, &window)) {
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


    

    void setStyle(DWORD style, bool activate, bool isEXStyle = false) {
        if (!isEXStyle) {
            if (activate) SetWindowLong(wndHandle, GWL_STYLE, GetWindowLong(wndHandle, GWL_STYLE) | style);
            else SetWindowLong(wndHandle, GWL_STYLE, GetWindowLong(wndHandle, GWL_STYLE) & ~style);
        }
        else {
            if (activate) SetWindowLong(wndHandle, GWL_EXSTYLE, GetWindowLong(wndHandle, GWL_EXSTYLE) | style);
            else SetWindowLong(wndHandle, GWL_EXSTYLE, GetWindowLong(wndHandle, GWL_EXSTYLE) & ~style);
        }
    }

    void updateWindowByConfig() {
        if (wndHandle == NULL || ignoreUpdateByConfig == true) return;

       
        float scale = getHWNDScale(wndHandle);

        //movable: handled in WindowProc
        if (checkNeedsUpdateAndReset("title")) SetWindowTextW(wndHandle, SK_String(config.data["title"]).toWString().c_str());

        bool isResizable = false;
        if (config.data.contains("resizable") == true) {
            isResizable = config.data["resizable"];
        }
        if (checkNeedsUpdateAndReset("resizable")) setStyle(WS_SIZEBOX, isResizable);

        if (checkNeedsUpdateAndReset("alwaysOnTop")) setAlwaysOnTop(config.data["alwaysOnTop"]);
        if (checkNeedsUpdateAndReset("maximizable")) setStyle(WS_MAXIMIZEBOX, config.data["maximizable"]);
        if (checkNeedsUpdateAndReset("minimizable")) setStyle(WS_MINIMIZEBOX, config.data["minimizable"]);
        if (checkNeedsUpdateAndReset("backgroundColor")) backgroundColor = config.data["backgroundColor"];
        /* WIP */ if (checkNeedsUpdateAndReset("focusable")) setStyle(WS_EX_NOACTIVATE, !config.data["focusable"], true);
        if (checkNeedsUpdateAndReset("skipTaskbar")) setStyle(WS_EX_APPWINDOW, config.data["skipTaskbar"], true);
        
        if (checkNeedsUpdateAndReset("frame")) {
            bool hasFrame = config.data["frame"];
            setStyle(WS_CAPTION, hasFrame);
            setStyle(WS_SYSMENU, hasFrame);
            setStyle(WS_OVERLAPPEDWINDOW, hasFrame);
            setStyle(WS_POPUP, !hasFrame);
        }

        if (checkNeedsUpdateAndReset("opacity")) {
            SK_Number clamped = SK_Number::clamp(config.data["opacity"], .0, 1.);
            SK_Number opacity = SK_Number::map(clamped, .0, 1., .0, 255.);
            BYTE opacityInt = opacity;
            SetLayeredWindowAttributes(wndHandle, 0, opacity, LWA_ALPHA);
        }

        if (checkNeedsUpdateAndReset("closable")) {
            HMENU hMenu = GetSystemMenu(wndHandle, FALSE);
            if (hMenu) {
                if (config.data["closable"] == false) EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
                else EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);
                DrawMenuBar(wndHandle);
            }
        }

        if (checkNeedsUpdateAndReset("thickFrame")) {
            setStyle(WS_THICKFRAME, config.data["thickFrame"]);
        }

        if (checkNeedsUpdateAndReset("oldStyle")) {
            int policy = (config.data["oldStyle"] ? DWMNCRP_DISABLED : DWMNCRP_ENABLED);
            DwmSetWindowAttribute(wndHandle, DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));
        }


     
        //everything below this comment should come last

        if (!isMaximized) {
            
            if (checkNeedsUpdateAndReset("center") && config.data["center"] == true) {
                RECT  wndRect;
                GetWindowRect(wndHandle, &wndRect);

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
           
            bool needsReposition = false;
            bool needsResize = false;
            if (checkNeedsUpdateAndReset("x") || checkNeedsUpdateAndReset("y")) needsReposition = true;
            if (checkNeedsUpdateAndReset("width") || checkNeedsUpdateAndReset("width")) needsResize = true;
            



            int w = config["width"] * scale;
            int h = config["height"] * scale;



            bool bypass = false;

            if (skg) {
                if (skg->onBeforeWndResize) {
                    ignoreUpdateByConfig = true;
                    SK_Point size = skg->onBeforeWndResize(this);
                    ignoreUpdateByConfig = false;

                    if (size.x == -2) bypass = true;

                    if (size.x > -1) w = size.x;
                    if (size.y > -1) h = size.y;
                }
            }

            if (!bypass) {
                if (config.data.contains("mainWindow") && config.data["mainWindow"] == false) {
                    if (needsReposition || needsResize) SetWindowPos(wndHandle, NULL, config.data["x"], config.data["y"], w, h, SWP_NOZORDER);
                }
            }
            
            if (needsResize) update();

            if (checkNeedsUpdateAndReset("show")) ShowWindow(wndHandle, (config["show"] ? SW_SHOW : SW_HIDE));
        }
        
        if (needsWindowUpdate()) {
            InvalidateRect(wndHandle, NULL, TRUE);
            UpdateWindow(wndHandle);
        }

        if (checkNeedsUpdateAndReset("fullscreen")) setFullscreen(config.data["fullscreen"]);
        if (checkNeedsUpdateAndReset("kiosk")) setFullscreen(config.data["kiosk"]);
    }
    
    void setAlwaysOnTop(bool flag, int level = 0, int relativeLevel = 0) {
        config.data["alwaysOnTop"] = flag;
        if (flag == true) SetWindowPos(wndHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        else SetWindowPos(wndHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);


        emitWndEvent(this, "always-on-top-changed", { {"isAlwaysOnTop", flag} });
    }

    void setFullscreen(bool activate) {
        if (!config.data["fullscreenable"]) return;

        if (!activate) {
            if (config["frame"] == true) config_updateTracker["frame"] = true;
            config_updateTracker["x"] = true;
            config_updateTracker["y"] = true;
            config_updateTracker["width"] = true;
            config_updateTracker["height"] = true;
            updateWindowByConfig();

            updateWebView();

            if (config.data["fullscreen"]) emitWndEvent(this, "leave-fullscreen", {});
            config.data["fullscreen"] = false;

            return;
        }


        DWORD style = GetWindowLong(wndHandle, GWL_STYLE);
        MONITORINFO monitor_info = { sizeof(monitor_info) };
        RECT wndRect;
        if (GetWindowRect(wndHandle, &wndRect) && GetMonitorInfo(MonitorFromWindow(wndHandle, MONITOR_DEFAULTTOPRIMARY), &monitor_info)) {
            SetWindowLong(wndHandle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(
                wndHandle, HWND_TOP, monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
                monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
                monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }

        updateWebView();

        emitWndEvent(this, "enter-fullscreen", {});
        config.data["fullscreen"] = true;
    }

    void handleWindowAction(const nlohmann::json& payload){
        SK_String action = "";
        if (payload.contains("action")) action = SK_String(payload["action"]);

        if (action == "beginMoveWindow") {
            ReleaseCapture();

            // Use current cursor position (screen coords)
            POINT pt;
            GetCursorPos(&pt);

            // Tell the window �the user pressed down on the title bar here�
            SendMessage(wndHandle, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(pt.x, pt.y));
        }
        else if (action == "close") { 
            SendMessage(wndHandle, WM_SYSCOMMAND, SC_CLOSE, 0);
        }
        else if (action == "focus") {
            if (IsIconic(wndHandle)) ShowWindow(wndHandle, SW_RESTORE); // if minimized
            ShowWindow(wndHandle, SW_SHOW);                      // make sure it's visible
            BringWindowToTop(wndHandle);
            SetForegroundWindow(wndHandle);                      // give it focus/activation
            SetActiveWindow(wndHandle);
        }
        else if (action == "blur") {
            // Windows doesn't have a direct "blur" for top-level windows.
            // Best effort: activate another window; if none, minimize this one.
            HWND other = GetWindow(wndHandle, GW_HWNDPREV);
            if (!other || !IsWindow(other)) other = GetWindow(wndHandle, GW_HWNDNEXT);
            if (other && other != wndHandle) {
                SetForegroundWindow(other);
            } else {
                ShowWindow(wndHandle, SW_MINIMIZE); // fallback so it's not active
            }
        }
        else if (action == "show") {
            ShowWindow(wndHandle, SW_SHOW);
        }
        else if (action == "hide") {
            ShowWindow(wndHandle, SW_HIDE);
        }
        else if (action == "maximize") {
            SendMessage(wndHandle, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        }
        else if (action == "unmaximize") {
            if (IsZoomed(wndHandle)) ShowWindow(wndHandle, SW_RESTORE); // only if currently maximized
        }
        else if (action == "minimize") {
            SendMessage(wndHandle, WM_SYSCOMMAND, SC_MINIMIZE, 0);
        }
        else if (action == "restore") {
            SendMessage(wndHandle, WM_SYSCOMMAND, SC_RESTORE, 0);
        }
    }
private:

};

END_SK_NAMESPACE
