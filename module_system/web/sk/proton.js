console.log('sk:proton')

sk_api.__protonjs = {
    window_uuid_counter: "0",
    next_window_uuid: () => {
        var counterAsInt = parseInt(sk_api.__protonjs.window_uuid_counter)
        counterAsInt++
        sk_api.__protonjs.window_uuid_counter = counterAsInt.toString()
        return sk_api.__protonjs.window_uuid_counter
    }
}

class BrowserWindow extends SK_Module_Root {
    constructor(opt = {}) {
        super('proton')

        this.__moduleInstanceConfig.__uuid = sk_api.__protonjs.next_window_uuid()

        this.defOpt = {
            "width": 800, //== [OK] ==//
            "height": 600, //== [OK] ==//
            "x": 0,
            "y": 0,
            "useContentSize": false,
            "center": true,
            "minWidth": 0, //== [OK] ==//
            "minHeight": 0, //== [OK] ==//
            "maxWidth": Infinity, //== [OK] ==//
            "maxHeight": Infinity, //== [OK] ==//
            "resizable": true, //== [OK] ==//
            "movable": true, //== [OK] ==//
            "minimizable": true,
            "maximizable": true,
            "closable": true,
            "focusable": true,
            "alwaysOnTop": false, //== [OK] ==//
            "fullscreen": false,
            "fullscreenable": true,
            "simpleFullscreen": false, // macOS exclusive
            "skipTaskbar": false,
            "kiosk": false,
            "title": "My SK Protom Window",
            "icon": null,
            "show": true,
            "frame": true,
            "parent": null,
            "modal": false,
            "acceptFirstMouse": false,
            "disableAutoHideCursor": false,
            "autoHideMenuBar": false,
            "enableLargerThanScreen": false,
            "backgroundColor": "#FFFFFF",
            "hasShadow": true,
            "opacity": 1.0,
            "darkTheme": false,
            "transparent": false,
            "type": "normal",
            "titleBarStyle": "default", // macOS exclusive (options: 'default', 'hidden', 'hiddenInset', 'customButtonsOnHover')
            "thickFrame": true, // Windows exclusive
            "vibrancy": "none", // macOS exclusive (options: 'appearance-based', 'light', 'dark', 'titlebar', 'selection', 'menu', 'popover', 'sidebar', 'medium-light', 'ultra-dark')
            "zoomToPageWidth": false, // macOS exclusive
            "tabbingIdentifier": null, // macOS exclusive
            "webPreferences": {
                "devTools": true,
                "nodeIntegration": false,
                "nodeIntegrationInWorker": false,
                "nodeIntegrationInSubFrames": false,
                "preload": null,
                "sandbox": false,
                "session": null,
                "partition": null,
                "zoomFactor": 1.0,
                "javascript": true,
                "webSecurity": true,
                "allowRunningInsecureContent": false,
                "images": true,
                "imageAnimationPolicy": "animate",
                "textAreasAreResizable": true,
                "webgl": true,
                "plugins": false,
                "experimentalFeatures": false,
                "scrollBounce": false, // macOS exclusive
                "enableBlinkFeatures": "",
                "disableBlinkFeatures": "",
                "defaultFontFamily": {
                    "standard": "Times New Roman",
                    "serif": "Times New Roman",
                    "sansSerif": "Arial",
                    "monospace": "Courier New",
                    "cursive": "Script",
                    "fantasy": "Impact",
                    "math": "Latin Modern Math"
                },
                "defaultFontSize": 16,
                "defaultMonospaceFontSize": 13,
                "minimumFontSize": 0,
                "defaultEncoding": "ISO-8859-1",
                "backgroundThrottling": true,
                "offscreen": false,
                "useSharedTexture": false,
                "contextIsolation": true,
                "webviewTag": false,
                "additionalArguments": [],
                "safeDialogs": false,
                "safeDialogsMessage": null,
                "disableDialogs": false,
                "navigateOnDragDrop": false,
                "autoplayPolicy": "no-user-gesture-required",
                "disableHtmlFullscreenWindowResize": false,
                "accessibleTitle": "",
                "spellcheck": true,
                "enableWebSQL": true,
                "v8CacheOptions": "code",
                "enablePreferredSizeMode": false,
                "paintWhenInitiallyHidden": true
            },
            "paintWhenInitiallyHidden": true,
            "throttleWhenBackground": true,
            "trafficLightPosition": { "x": 10, "y": 10 }, // macOS exclusive (controls the position of the traffic light buttons in frameless windows)
            "roundedCorners": true, // macOS exclusive
            "hiddenInMissionControl": false, // macOS exclusive
        }

        this.defOpt = { ...this.defOpt, ...opt }
            
        this.sync('construct', { constructorOpts: this.defOpt })

        /*const proxy = new Proxy(this, {
            set(target, key, val) {
                if (!this[key] && !this.defOpt[key]) return console.log('Invalid call')

                if (this.defOpt[key]) {
                    this.defOpt[key] = value
                }
            }
        });*/
    }



    static getAllWindows() {
        return this.async('getAllWindows')
    }

    static getFocusedWindow() {
        return this.async('getFocusedWindow')
    }

    static fromWebContents(webContents) {

    }

    static fromId(id) {

    }

    get webContents() {
        return;
    }

    get ids() {
        return;
    }

    get tabbingIdentifier() {
        return;
    }


    set autoHideMenuBar(val) {

    }

    get autoHideMenuBar() {
        return;
    }


    set simpleFullScreen(val) {

    }
    get simpleFullScreen() {
        return;
    }


    set fullScreen(val) {

    }
    get fullScreen() {
        return;
    }


    set focusable(val) {

    }
    get focusable() {
        return;
    }


    set visibleOnAllWorkspaces(val) {

    }

    get visibleOnAllWorkspaces() {
        if (sk_api.staticInfo.machine.os == "win") return false //always returns false on windows

        //macos result here

        return;
    }


    set shadow(val) {

    }
    get shadow() {
        return;
    }


    set menuBarVisible(val) {

    }

    get menuBarVisible() {
        return;
    }


    set kiosk(val) {

    }
    get kiosk() {
        return;
    }


    set documentEdited(val) {

    }
    get documentEdited() {
        return;
    }


    set representedFilenames(val) {

    }

    get representedFilenames() {
        return;
    }


    set title(val) {

    }
    get title() {
        return;
    }

    set minimizable(val) { this.sync('configure', { attribute: 'minimizable', value: val }) }
    get minimizable() { return this.sync('configure', { attribute: 'minimizable', read: true }) }


    set maximizable(val) { this.sync('configure', { attribute: 'maximizable', value: val }) }
    get maximizable() { return this.sync('configure', { attribute: 'maximizable', read: true }) }


    set fullScreenable(val) {

    }

    get fullScreenable() {
        return;
    }


    set resizable(val) { this.sync('configure', { attribute: 'resizable', value: val }) }
    get resizable() { return this.sync('configure', { attribute: 'resizable', read: true }) }


    set closable(val) { this.sync('configure', { attribute: 'closable', value: val }) }
    get closable() { return this.sync('configure', { attribute: 'closable', read: true }) }


    set movable(val) { this.sync('configure', { attribute: 'movable', value: val }) }
    get movable() { return this.sync('configure', { attribute: 'movable', read: true }) }

    set excludedFromShownWindowsMenu(val) {

    }
    get excludedFromShownWindowsMenu() {
        return;
    }

    set accessibleTitle(val) {

    }
    get accessibleTitle() {
        return;
    }


    //methods
    destroy() {

    }

    close() {

    }

    focus() {

    }

    blur() {

    }

    isFocused() {

    }

    isDestroyed() {

    }

    show() {

    }

    showInactive() {

    }

    hide() {

    }

    isVisible() {

    }

    isModal() {

    }

    maximize() {

    }

    unmaximize() {

    }

    isMaximized() {

    }

    minimize() {

    }

    restore() {

    }

    isMinimized() {

    }

    setFullScreen(flag) {

    }

    isFullScreen() {

    }

    setSimpleFullScreen(flag) {

    }

    isSimpleFullScreen() {

    }

    isNormal() {

    }

    setAspectRatio(aspectRatio, extraSize) {

    }

    setBackgroundColor(backgroundColor) {

    }

    previewFile(path, displayName) {

    }

    closeFilePreview() {

    }

    setBounds(bounds, animate) {

    }

    getBounds() {

    }

    getBackgroundColor() {

    }

    setContentBounds(bounds, animate) {

    }

    getContentBounds() {

    }

    getNormalBounds() {

    }

    setEnabled(enable) {

    }

    isEnabled() {

    }

    setSize(width, height, animate) {

    }

    getSize() {

    }

    setContentSize(width, height, animate) {

    }

    getContentSize() {

    }

    setMinimumSize(width, height) {

    }

    getMinimumSize() {

    }

    setMaximumSize(width, height) {

    }

    getMaximumSize() {

    }

    setResizable(resizable) {

    }

    isResizable() {

    }

    setMovable(movable) {

    }

    isMovable() {

    }

    setMinimizable(minimizable) {

    }

    isMinimizable() {

    }

    setMaximizable(maximizable) {

    }

    isMaximizable() {

    }

    setFullScreenable(fullscreenable) {

    }

    isFullScreenable() {

    }

    setClosable(closable) {

    }

    isClosable() {

    }

    setHiddenInMissionControl(hidden) {

    }

    isHiddenInMissionControl() {

    }

    setAlwaysOnTop(flag, level, relativeLevel) { this.sync('configure', { attribute: 'setAlwaysOnTop', value: { flag: flag, level: level, relativeLevel: relativeLevel } }) } //== [OK] ==//

    isAlwaysOnTop() { return this.sync('configure', { attribute: 'alwaysOnTop', read: true }) } //== [OK] ==//

    moveAbove(mediaSourceId) {

    }

    moveTop() {

    }

    center() { this.sync('configure', { attribute: 'center', value: true }) } //== [OK] ==//

    setPosition(x, y, animate) {

    }

    getPosition() {

    }

    setTitle(title) {

    }

    getTitle() {

    }

    setSheetOffset(offsetY, offsetX) {

    }

    flashFrame(flag) {

    }

    setSkipTaskbar(skip) {

    }

    setKiosk(flag) {

    }

    isKiosk() {

    }

    isTabletMode() {

    }

    getMediaSourceId() {

    }

    getNativeWindowHandle() {

    }

    hookWindowMessage(message, callback) {

    }

    isWindowMessageHooked(message) {

    }

    unhookWindowMessage(message) {

    }

    unhookAllWindowMessages() {

    }

    setRepresentedFilename(filename) {

    }

    getRepresentedFilename() {

    }

    setDocumentEdited(edited) {

    }

    isDocumentEdited() {

    }

    focusOnWebView() {

    }

    blurWebView() {

    }

    capturePage(rect, opts) {

    }

    loadURL(url, options) {

    }

    loadFile(filePath, options) {

    }

    reload() {

    }

    setMenu() {

    }

    removeMenu() {

    }

    setProgressBar(progress, options) {

    }

    setOverlayIcon(overlay, description) {

    }

    invalidateShadow() {

    }

    setHasShadow(hasShadow) {

    }

    hasShadow() {

    }

    setOpacity(opacity) {

    }

    getOpacity() {

    }

    setShape(rects) {

    }

    setThumbarButtons(buttons) {

    }

    setThumbnailClip(region) {

    }

    setThumbnailToolTip(toolTip) {

    }

    setAppDetails(options) {

    }

    showDefinitionForSelection() {

    }

    setIcon(icon) {

    }

    setWindowButtonVisibility(visible) {

    }

    setAutoHideMenuBar(hide) {

    }

    isMenuBarAutoHide() {

    }

    setMenuBarVisibility(visible) {

    }

    isMenuBarVisible() {

    }

    setVisibleOnAllWorkspaces(visible, options) {

    }

    isVisibleOnAllWorkspaces() {

    }

    setIgnoreMouseEvents(ignore, options) {

    }

    setContentProtection(enable) {

    }

    setFocusable(focusable) {

    }

    isFocusable() {

    }

    setParentWindow(parent) {

    }

    getParentWindow() {

    }

    getChildWindows() {

    }

    setAutoHideCursor(autoHide) {

    }

    selectPreviousTab() {

    }

    selectNextTab() {

    }

    showAllTabs() {

    }

    mergeAllWindows() {

    }

    moveTabToNewWindow() {

    }

    toggleTabBar() {

    }

    addTabbedWindow(browserWindow) {

    }

    setVibrancy(type) {

    }

    setBackgroundMaterial(material) {

    }

    setWindowButtonPosition(position) {

    }

    getWindowButtonPosition() {

    }

    setTouchBar(touchBar) {

    }

    setTitleBarOverlay(options) {

    }

}


module.exports = {
    BrowserWindow
}