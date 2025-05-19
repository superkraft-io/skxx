#pragma once

#include "../sk_common.hpp"

#include "root/sk_window_root.hpp"

#if defined(SK_OS_windows)
    #include "windows/sk_window_windows.hpp"
#elif defined(SK_OS_macos) || defined(SK_OS_ios)
    #include "macos/sk_window_macos.hpp"
#endif

BEGIN_SK_NAMESPACE

class SK_WebViewResourceHandler;


class SK_Window_WebView_Counter {
public:
    #if defined(SK_OS_windows)
        std::unordered_map<std::string, std::vector<HWND>> list;
   

        bool addWebviewToCounterList(const SK_String& windowClassName, HWND hwnd) {
            auto it = list.find(windowClassName);

            if (it == list.end()) {
                list[windowClassName] = { hwnd };
                return true;
            }
            else {
                auto& hwnd_list = it->second;
                if (std::find(hwnd_list.begin(), hwnd_list.end(), hwnd) == hwnd_list.end()) {
                    hwnd_list.push_back(hwnd);
                    return true;
                }
                else {
                    return false;
                }
            }
        }

        HWND getWebViewForWindow(const SK_String& windowClassName) {
            auto it = list.find(windowClassName);

            if (it == list.end() || it->second.empty()) {
                return nullptr;
            }

            return it->second[it->second.size() - 1];
        }
#elif defined(SK_OS_macos) || defined(SK_OS_ios)
#endif

};

class SK_Window_Mngr {
public:
	SK_Global* skg;

	SK_Window_WebView_Counter wvCounter;

	unsigned int wndIdx = 0;
	
	std::unordered_map<std::string, SK_Window*> list;
    
    SK_Window_Mngr(){
        int x = 0;
    }
    
	SK_Window* findWindowByClassName(const SK_String& windowClassName) {
		auto it = list.find(windowClassName);

		if (it == list.end() || it->second == nullptr) {
			return nullptr;
		}

		return it->second;
	};

	using SK_Window_Create_Callback = std::optional<std::function<void(SK_Window*)>>;

	SK_Window_Mngr(SK_Global* _skg) {
		skg = _skg;
        
        skg->onWindowFocusChanged = [&](SK_Window* wnd, const bool& focused) {
			if (focused) updateAllWindows(); //This will fix the ussue
		};

        skg->onFindWindowByClassName = [&](const SK_String& windowClassName) {
			return findWindowByClassName(windowClassName);
		};

        skg->onFindWindowByTag = [&](const SK_String& windowTag) {
			return findWindowByTag(windowTag);
		};


		/*******************/


		#if defined(SK_OS_windows)
            skg->updateWebViewHWNDListForView = [&](const SK_String& windowClassName) {
				updateAllWebViewHandlesForView(windowClassName);
			};

            skg->getWebview2HWNDForWindow = [&](const SK_String& windowClassName) {
				return wvCounter.getWebViewForWindow(windowClassName);
			};
		#endif

        skg->resizeAllMainWindowViews = [&](int x, int y, int w, int h, float scale) {
			#if defined(SK_MODE_DEBUG)
				for (auto it = list.begin(); it != list.end(); ++it) {
					if (it->second) {
						SK_Window* wnd = it->second;
						
						if (wnd->config["mainWindow"] == true) {
                            wnd->config.data["scale"]  = scale;
							
                            wnd->config.data["left"] = x;
                            wnd->config_updateTracker["left"] = true;
                            
                            wnd->config.data["top"] = y;
                            wnd->config_updateTracker["top"] = true;
                            
                            
							wnd->config.data["width"] = w;
                            wnd->config_updateTracker["width"] = true;
                            
							wnd->config.data["height"] = h;
                            wnd->config_updateTracker["height"] = true;
                            
                            wnd->updateWindowByConfig();
                            
                            wnd->updateWebView();
						}
					}
				}
			#endif
		};
	}

	~SK_Window_Mngr() {
		destroyAllWindows();
	}

	SK_Window* newWindow(SK_Window_Create_Callback cb = nullptr) {
		wndIdx++;

		SK_Window* wnd = new SK_Window();
		wnd->skg = skg;
		wnd->ipc->skg = skg;
		wnd->webview.skg = skg;

		wnd->initialize(wndIdx);

        std::string wcn = wnd->windowClassName;
		list[wcn] = wnd;

		if (cb != nullptr) (*cb)(wnd);

		return wnd;
	};

	SK_Window* findWindowByTag(const SK_String& tag) {
		for (auto it = list.begin(); it != list.end(); ++it) {
			if (it->second) {
				SK_Window* wnd = it->second;
				if (wnd->tag == tag) return wnd;
			}
		}

		return nullptr;
	};


	void updateAllWindows() {
		return;
		// Iterate using iterators
		for (auto it = list.begin(); it != list.end(); ++it) {
			if (it->second) {
				SK_Window* wnd = it->second;
				wnd->updateWebView();
			}
		}

	};

	#if defined(SK_OS_windows)
		void updateAllWebViewHandlesForView(const SK_String& windowClassName) {
			auto it = list.find(windowClassName);

			if (it == list.end() || it->second == nullptr) {
				return;
			}

			SK_Window* window = it->second;

			HWND child = GetWindow(window->wndHandle, GW_CHILD);

			while (child) {
				char className[256];
				int len = GetClassNameA(child, className, sizeof(className) - 1);
				className[len] = '\0';

				std::string _className = std::string(className);

				if (_className == "Chrome_WidgetWin_0") {
					if (!wvCounter.addWebviewToCounterList(window->windowClassName, child)) {
						//Failed to add to counter list
					}
					else {
						//Successfully added to counter list
						int x = 0;
					}
				}

				// Get the next child window
				child = GetWindow(child, GW_HWNDNEXT);
			}
		}
	#endif
    
    
    /*void removeWindow(const SK_String& wndTag){
        for (auto it = list.begin(); it != list.end(); ++it) {
            if (it->second) {
                SK_Window* wnd = it->second;
                if (wnd->tag == wndTag){
                    
                    return;
                };
            }
    }*/


    void destroyAllWindows() {
        if (list.size() == 0) return;
        
        
        for (std::unordered_map<std::string, SK_Window*>::iterator it = list.begin(); it != list.end(); ++it) {
            SK_Window* wnd = it->second;
            
            delete wnd;
            
            it->second = nullptr;
        }
        
        skg->mainWindow = nullptr;
        
		list.clear();
	}
private:

};


END_SK_NAMESPACE
