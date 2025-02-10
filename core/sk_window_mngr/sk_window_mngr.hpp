#pragma once

#include "../sk_common.hpp"


BEGIN_SK_NAMESPACE

class SK_WebViewResourceHandler;


class SK_Window_WebView_Counter {
public:
    #if defined(SK_OS_windows)
        static inline std::unordered_map<std::string, std::vector<HWND>> list;
   

        static inline bool addWebviewToCounterList(const SK_String& windowClassName, HWND hwnd) {
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

        static inline HWND getWebViewForWindow(const SK_String& windowClassName) {
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
	unsigned int wndIdx = 0;
	
	std::unordered_map<std::string, SK_Window*> list;

	SK_Window* findWindowByClassName(const SK_String& windowClassName) {
		auto it = list.find(windowClassName);

		if (it == list.end() || it->second == nullptr) {
			return nullptr;
		}

		return it->second;
	};

	using SK_Window_Create_Callback = std::optional<std::function<void(SK_Window*)>>;

	SK_Window_Mngr() {
		SK_Common::onWindowFocusChanged = [&](SK_Window* wnd, const bool& focused) {
			if (focused) updateAllWindows(); //This will fix the ussue
		};

		SK_Common::onFindWindowByClassName = [&](const SK_String& windowClassName) {
			return findWindowByClassName(windowClassName);
		};

		SK_Common::onFindWindowByTag = [&](const SK_String& windowTag) {
			return findWindowByTag(windowTag);
		};


		/*******************/


		#if defined(SK_OS_windows)
			SK_Common::updateWebViewHWNDListForView = [&](const SK_String& windowClassName) {
				updateAllWebViewHandlesForView(windowClassName);
			};

			SK_Common::getWebview2HWNDForWindow = [&](const SK_String& windowClassName) {
				return SK_Window_WebView_Counter::getWebViewForWindow(windowClassName);
			};
		#endif

		SK_Common::resizeAllMianWindowView = [&](int x, int y, int w, int h, float scale) {
			#if defined(SK_MODE_DEBUG)
				for (auto it = list.begin(); it != list.end(); ++it) {
					if (it->second) {
						SK_Window* wnd = it->second;
						
						if (wnd->config["mainWindow"] == true) {
							wnd->config["left"] = x;
							wnd->config["top"] = y;
							wnd->config["width"] = w;
							wnd->config["height"] = h;
							wnd->config["scale"] = scale;

							//if (wnd->webview.webview != nullptr) {
							//	wnd->update();
							//}
						}

						
					}
				}
			#endif
		};
	}

	~SK_Window_Mngr() {
		for (auto& [key, wnd] : list) {
			delete wnd;
		}
		list.clear();
	}

	SK_Window* newWindow(SK_Window_Create_Callback cb = nullptr) {
		wndIdx++;

		SK_Window* wnd = new SK_Window();

		wnd->initialize(wndIdx);

		list[wnd->windowClassName] = wnd;

		if (cb != nullptr) (*cb)(wnd);

		return wnd;

		return nullptr;
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
				wnd->update();
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

			HWND child = GetWindow(window->hwnd, GW_CHILD);

			while (child) {
				char className[256];
				int len = GetClassNameA(child, className, sizeof(className) - 1);
				className[len] = '\0';

				std::string _className = std::string(className);

				if (_className == "Chrome_WidgetWin_0") {
					if (!SK_Window_WebView_Counter::addWebviewToCounterList(window->windowClassName, child)) {
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
private:

};


END_SK_NAMESPACE
