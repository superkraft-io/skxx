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

class NatoveImage {
    createEmpty() {

    }

    createThumbnailFromPath() {

    }

    createFromPath() {

    }

    createFromBitmap() {

    }

    createFromBuffer() {

    }

    createFromDataURL() {

    }

    createFromNamedImage() {

    }

    toPNG(opt) {

    }

    toJPEG(opt) {

    }

    toBitmap(opt) {

    }

    toDataURL(opt) {

    }

    getBitmap(opt) {

    }

    getNativeHandle() {

    }

    isEmpty() {

    }

    getSize() {

    }

    setTemplateImage() {

    }

    isTemplateImage() {

    }

    crop() {

    }

    resize() {

    }

    getAspectRatio(scale) {

    }

    getScaleFactors() {

    }

    addRepresentation(opt) {

    }

    set isMacTemplateImage(val) {

    }

    get isMacTemplateImage() {
        return 0
    }
}

class BrowserWindow extends SK_Module_Root {
    constructor(opt = {}) {
        super('proton')

        this.events = {}


        this.__moduleInstanceConfig.__uuid = sk_api.__protonjs.next_window_uuid()

        this.defOpt = {
            //SK Added features
            "oldStyle": false, //[OK] win       Windows exclusive
            "roundness": -1.0, //Should dictates the roundness of a the window, but isn't. Why? because it must use a mask, and redrawing the mask during window resize is too inefficient.

            //ElectronJS compatible features
            "title": "My SK++ ProtonJS Window", //[OK] win, macos
            "backgroundColor": "#FFFFFF", //[OK] win, macos
            "width": 800,           //[OK] win, macos
            "height": 600,          //[OK] win, macos
            "x": 0,                 //[OK] win, macos
            "y": 0,                 //[OK] win, macos
            "center": true,         //[OK] win, macos
            "minWidth": 0,          //[OK] win, macos
            "minHeight": 0,         //[OK] win, macos
            "maxWidth": -1,         //[OK] win, macos
            "maxHeight": -1,        //[OK] win, macos
            "resizable": true,      //[OK] win, macos
            "movable": true,        //[OK] win, macos
            "minimizable": true,    //[OK] win, macos
            "maximizable": true,    //[OK] win, macos
            "closable": true,       //[OK] win, macos
            "show": true,           //[OK] win, macos
            "frame": true,          //[OK] win, macos
            "transparent": false,   //[OK] win, macos
            "opacity": 1.0,         //[OK] win, macos
            "alwaysOnTop": false,   //[OK] win
            "skipTaskbar": false,   //[OK] win
            "kiosk": false,         //[OK] win
            "fullscreenable": true, //[OK] win, macos
            "fullscreen": false,    //[OK] win, macos

            "thickFrame": true,     //[OK] win      Windows exclusive 


            "icon": null,
            "useContentSize": false,
            "focusable": true,
            "parent": null,
            "modal": false,
            "disableAutoHideCursor": false,
            "autoHideMenuBar": false,
            "type": "normal",
            "paintWhenInitiallyHidden": true, 


            "hasShadow": true,  //Seems to do nothing on Windows 11
            "darkTheme": false, //Seems to do nothing on Windows 11
            "throttleWhenBackground": true, //not documented?


            //MacOS Exclusives
            "acceptFirstMouse": false,
            "enableLargerThanScreen": false,
            "simpleFullscreen": false,
            "titleBarStyle": "default", //options: 'default', 'hidden', 'hiddenInset', 'customButtonsOnHover'
            "zoomToPageWidth": false,
            "tabbingIdentifier": null,
            "trafficLightPosition": { "x": 10, "y": 10 }, //controls the position of the traffic light buttons in frameless windows
            "vibrancy": "none", //options: 'appearance-based', 'light', 'dark', 'titlebar', 'selection', 'menu', 'popover', 'sidebar', 'medium-light', 'ultra-dark'
            "hiddenInMissionControl": false,
            
            "roundedCorners": true, //Unused. Use the ProtonJS attribute "roundness" instead.





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
        }

        this.defOpt = { ...this.defOpt, ...opt }
        
        
        sk_api.ipc.on('sk:proton.js::windowEvent::' + this.__moduleInstanceConfig.__uuid, (res, respondWith)=>{
            this.emit(res, respondWith)
        })
        
        this.sync('construct', { constructorOpts: this.defOpt })
    }
    

    emit(res, respondWith) {
        var listenerCB = this.events[res.eventID]
        if (!listenerCB) return

        var opt = {...res.data, ...{}}

        if (res.eventID === 'close'){
            opt.returnValue = '<null>'
        }

        listenerCB(opt)

        var responseObj = {}
        if (opt.returnValue){
            responseObj.returnValue = opt.returnValue
            if (responseObj.returnValue != false && responseObj.returnValue != true) responseObj.returnValue = '<null>'
        }
        
        respondWith({returnValue: responseObj.returnValue})
    }

    on(eventID, cb) {
        var listener = this.events[eventID]
        if (!listener) this.events[eventID] = cb
    }

    off(eventID, callback) {
        delete this.events[eventID]
    }






    setAttrSync(attribute, value) { this.sync('configure', { attribute: attribute, value: value }) }
    getAttrSync(attribute, value) { return this.sync('configure', { attribute: attribute, read: true }) }

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

    


    set fullscreen(val) { this.setAttrSync('fullscreen', val) }
    get fullscreen() { return this.getAttrSync('fullscreen') }


    set focusable(val) { this.setAttrSync('focusable', val) }
    get focusable() { return this.getAttrSync('focusable') }


    set title(val) { this.setAttrSync('title', val) }
    get title() { return this.getAttrSync('title') }


    set minimizable(val) { this.setAttrSync('minimizable', val) }
    get minimizable() { return this.getAttrSync('minimizable') }


    set maximizable(val) { this.setAttrSync('maximizable', val) }
    get maximizable() { return this.getAttrSync('maximizable') }

    set fullscreenable(val) { this.setAttrSync('fullscreenable', val) }
    get fullscreenable() { return this.getAttrSync('fullscreenable') }


    set resizable(val) { this.setAttrSync('resizable', val) }
    get resizable() { return this.getAttrSync('resizable') }


    set closable(val) { this.setAttrSync('closable', val) }
    get closable() { return this.getAttrSync('closable') }


    set movable(val) { this.setAttrSync('movable', val) }
    get movable() { return this.getAttrSync('movable') }

    set backgroundColor(val) { this.setAttrSync('backgroundColor', val) }
    get backgroundColor() { return this.getAttrSync('backgroundColor') }
    
    set transparent(val) { this.setAttrSync('transparent', val) }
    get transparent() { return this.getAttrSync('transparent') }

    set skipTaskbar(val) { this.setAttrSync('skipTaskbar', val) }
    get skipTaskbar() { return this.getAttrSync('skipTaskbar') }

    set frame(val) { this.setAttrSync('frame', val) }
    get frame() { return this.getAttrSync('frame') }
    
    set thickFrame(val) { this.setAttrSync('thickFrame', val) }
    get thickFrame() { return this.getAttrSync('thickFrame') }

    set opacity(val) { this.setAttrSync('opacity', val) }
    get opacity() { return this.getAttrSync('opacity') }
    
    set minWidth(val) { this.setAttrSync('minWidth', val) }
    get minWidth() { return this.getAttrSync('minWidth') }
    
    set maxWidth(val) { this.setAttrSync('maxWidth', val) }
    get maxWidth() { return this.getAttrSync('maxWidth') }
    
    set minHeight(val) { this.setAttrSync('minHeight', val) }
    get minHeight() { return this.getAttrSync('minHeight') }
    
    set maxHeight(val) { this.setAttrSync('maxHeight', val) }
    get maxHeight() { return this.getAttrSync('maxHeight') }

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
        this.fullscreen = flag
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

    setBackgroundColor(val) {
        this.backgroundColor = val
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

    setFullScreenable(val) { fullscreenable = val }
    isFullScreenable() { return fullscreenable }

    setClosable(closable) {

    }

    isClosable() {

    }

    setHiddenInMissionControl(hidden) {

    }

    isHiddenInMissionControl() {

    }

    setAlwaysOnTop(flag, level, relativeLevel) { this.setAttrSync('setAlwaysOnTop', { flag: flag, level: level, relativeLevel: relativeLevel }) } //[OK] win

    isAlwaysOnTop() { return this.getAttrSync('alwaysOnTop') } //[OK] win

    moveAbove(mediaSourceId) {

    }

    moveTop() {

    }

    center() { this.setAttrSync('center', true) } //[OK] win

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
        skipTaskbar = skip
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

    setOpacity(val) { this.opacity = val }
    getOpacity() { return this.opacity }

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
