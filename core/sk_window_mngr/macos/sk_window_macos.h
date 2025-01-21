#pragma once

#include "../root/sk_window_root.hxx"

class SK_Window : public SK_Window_Root{
public:
    NSWindow *window;
    id resizeObserver;

    void initialize(unsigned int _wndIdx) override;
    void create();
    int applyTo(NSWindow* wnd);
    void createWebView();
    
    void setTransparent(bool _transparent);
private:
    void handleWindowResize(NSNotification* notification);
};
