console.log('sk:proton')

sk_api.__protonjs = {
    window_uuid_counter: "0",
    next_window_uuid: () => {
        var counterAsInt = parseInt(sk_api.__protonjs.window_uuid_counter)
        counterAsInt++
        sk_api.__protonjs.window_uuid_counter = counterAsInt.toString()
        return sk_api.__protonjs.window_uuid_counter
    },

    windows: {}
}

class NativeImage {
    createEmpty() {
        throw 'Not implemented yet'
    }

    createThumbnailFromPath() {
        throw 'Not implemented yet'
    }

    createFromPath() {
        throw 'Not implemented yet'
    }

    createFromBitmap() {
        throw 'Not implemented yet'
    }

    createFromBuffer() {
        throw 'Not implemented yet'
    }

    createFromDataURL() {
        throw 'Not implemented yet'
    }

    createFromNamedImage() {
        throw 'Not implemented yet'
    }

    toPNG(opt) {
        throw 'Not implemented yet'
    }

    toJPEG(opt) {
        throw 'Not implemented yet'
    }

    toBitmap(opt) {
        throw 'Not implemented yet'
    }

    toDataURL(opt) {
        throw 'Not implemented yet'
    }

    getBitmap(opt) {
        throw 'Not implemented yet'
    }

    getNativeHandle() {
        throw 'Not implemented yet'
    }

    isEmpty() {
        throw 'Not implemented yet'
    }

    getSize() {
        throw 'Not implemented yet'
    }

    setTemplateImage() {
        throw 'Not implemented yet'
    }

    isTemplateImage() {
        throw 'Not implemented yet'
    }

    crop() {
        throw 'Not implemented yet'
    }

    resize() {
        throw 'Not implemented yet'
    }

    getAspectRatio(scale) {
        throw 'Not implemented yet'
    }

    getScaleFactors() {
        throw 'Not implemented yet'
    }

    addRepresentation(opt) {
        throw 'Not implemented yet'
    }

    set isMacTemplateImage(val) {
        throw 'Not implemented yet'
    }

    get isMacTemplateImage() {
        return 0
    }
}

class BrowserWindow extends SK_Module_Root {
    constructor(opt = {}) {
        super('proton')

        this.listeners = {}


        this.__moduleInstanceConfig.__target = 'window'
        this.__moduleInstanceConfig.__uuid = opt.id || sk_api.__protonjs.next_window_uuid()

        this.defOpt = { ...BrowserWindow.getDefOpts(), ...opt }
        
        
        sk_api.ipc.on('sk:proton.js::windowEvent::' + this.__moduleInstanceConfig.__uuid, (res, respondWith)=>{
            console.log(res)
            this.emit(res, respondWith)
        })
        
        this.sync('construct', { constructorOpts: this.defOpt })

        sk_api.__protonjs.windows[this.__moduleInstanceConfig.__uuid] = this
        sk_api.__protonjs.app.emit({eventID: 'browser-window-created', data: {window: this}}, ()=>{})
    }

    static getDefOpts(){
        return {
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
    }
    

    emit(res, respondWith) {
        var listenerCB = this.listeners[res.eventID]
        
        if (res.eventID === 'blur') sk_api.__protonjs.app.emit({eventID: 'browser-window-blur', data: {window: this}}, ()=>{})
        if (res.eventID === 'focus') sk_api.__protonjs.app.emit({eventID: 'browser-window-focus', data: {window: this}}, ()=>{})
        

       
        if (!listenerCB){
            respondWith({})
            return
        }

        
        var responseObj = {}


        var opt = {...res.data, ...{}}

        var preventables = [
            'close',
            'will-resize',
            'will-move',
            'page-title-updated',
            'system-context-menu'
        ]

        var responseObj = {}

        
        if (preventables.includes(res.eventID)){
            opt.preventDefault = ()=>{ responseObj.defaultPrevented = true }
        }


        listenerCB(opt)
        
        respondWith(responseObj)        
    }

    on(eventID, cb) {
        var listener = this.listeners[eventID]
        if (!listener) this.listeners[eventID] = cb
    }

    off(eventID, callback) {
        delete this.listeners[eventID]
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
        throw 'Not implemented yet'
    }

    static fromId(id) {
        throw 'Not implemented yet'
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

    set alwaysOnTop(val) { this.setAttrSync('alwaysOnTop', val) }
    get alwaysOnTop() { return this.getAttrSync('alwaysOnTop') }

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
        throw 'Not implemented yet'
    }

    get autoHideMenuBar() {
        return;
    }


    set simpleFullScreen(val) {
        throw 'Not implemented yet'
    }
    get simpleFullScreen() {
        return;
    }

    
    set visibleOnAllWorkspaces(val) {
        throw 'Not implemented yet'
    }

    get visibleOnAllWorkspaces() {
        if (sk_api.staticInfo.machine.os == "win") return false //always returns false on windows

        //macos result here

        return;
    }


    set shadow(val) {
        throw 'Not implemented yet'
    }
    get shadow() {
        return;
    }


    set menuBarVisible(val) {
        throw 'Not implemented yet'
    }

    get menuBarVisible() {
        return;
    }


    set kiosk(val) {
        throw 'Not implemented yet'
    }
    get kiosk() {
        return;
    }


    set documentEdited(val) {
        throw 'Not implemented yet'
    }
    get documentEdited() {
        return;
    }


    set representedFilenames(val) {
        throw 'Not implemented yet'
    }

    get representedFilenames() {
        return;
    }


   

    set excludedFromShownWindowsMenu(val) {
        throw 'Not implemented yet'
    }
    get excludedFromShownWindowsMenu() {
        return;
    }

    set accessibleTitle(val) {
        throw 'Not implemented yet'
    }
    get accessibleTitle() {
        return;
    }


    //methods
    destroy() {
        throw 'Not implemented yet'
    }

    close() {
        throw 'Not implemented yet'
    }

    focus() {
        throw 'Not implemented yet'
    }

    blur() {
        throw 'Not implemented yet'
    }

    isFocused() {
        throw 'Not implemented yet'
    }

    isDestroyed() {
        throw 'Not implemented yet'
    }

    show() {
        throw 'Not implemented yet'
    }

    showInactive() {
        throw 'Not implemented yet'
    }

    hide() {
        throw 'Not implemented yet'
    }

    isVisible() {
        throw 'Not implemented yet'
    }

    isModal() {
        throw 'Not implemented yet'
    }

    maximize() {
        throw 'Not implemented yet'
    }

    unmaximize() {
        throw 'Not implemented yet'
    }

    isMaximized() {
        throw 'Not implemented yet'
    }

    minimize() {
        throw 'Not implemented yet'
    }

    restore() {
        throw 'Not implemented yet'
    }

    isMinimized() {
        throw 'Not implemented yet'
    }

    setFullScreen(flag) {
        this.fullscreen = flag
    }

    isFullScreen() {
        throw 'Not implemented yet'
    }

    setSimpleFullScreen(flag) {
        throw 'Not implemented yet'
    }

    isSimpleFullScreen() {
        throw 'Not implemented yet'
    }

    isNormal() {
        throw 'Not implemented yet'
    }

    setAspectRatio(aspectRatio, extraSize) {
        throw 'Not implemented yet'
    }

    setBackgroundColor(val) {
        this.backgroundColor = val
    }

    previewFile(path, displayName) {
        throw 'Not implemented yet'
    }

    closeFilePreview() {
        throw 'Not implemented yet'
    }

    setBounds(bounds, animate) {
        throw 'Not implemented yet'
    }

    getBounds() {
        throw 'Not implemented yet'
    }

    getBackgroundColor() {
        throw 'Not implemented yet'
    }

    setContentBounds(bounds, animate) {
        throw 'Not implemented yet'
    }

    getContentBounds() {
        throw 'Not implemented yet'
    }

    getNormalBounds() {
        throw 'Not implemented yet'
    }

    setEnabled(enable) {
        throw 'Not implemented yet'
    }

    isEnabled() {
        throw 'Not implemented yet'
    }

    setSize(width, height, animate) {
        throw 'Not implemented yet'
    }

    getSize() {
        throw 'Not implemented yet'
    }

    setContentSize(width, height, animate) {
        throw 'Not implemented yet'
    }

    getContentSize() {
        throw 'Not implemented yet'
    }

    setMinimumSize(width, height) {
        throw 'Not implemented yet'
    }

    getMinimumSize() {
        throw 'Not implemented yet'
    }

    setMaximumSize(width, height) {
        throw 'Not implemented yet'
    }

    getMaximumSize() {
        throw 'Not implemented yet'
    }

    setResizable(resizable) {
        throw 'Not implemented yet'
    }

    isResizable() {
        throw 'Not implemented yet'
    }

    setMovable(movable) {
        throw 'Not implemented yet'
    }

    isMovable() {
        throw 'Not implemented yet'
    }

    setMinimizable(minimizable) {
        throw 'Not implemented yet'
    }

    isMinimizable() {
        throw 'Not implemented yet'
    }

    setMaximizable(maximizable) {
        throw 'Not implemented yet'
    }

    isMaximizable() {
        throw 'Not implemented yet'
    }

    setFullScreenable(val) { fullscreenable = val }
    isFullScreenable() { return fullscreenable }

    setClosable(closable) {
        throw 'Not implemented yet'
    }

    isClosable() {
        throw 'Not implemented yet'
    }

    setHiddenInMissionControl(hidden) {
        throw 'Not implemented yet'
    }

    isHiddenInMissionControl() {
        throw 'Not implemented yet'
    }

    setAlwaysOnTop(flag, level, relativeLevel) { this.setAttrSync('setAlwaysOnTop', { flag: flag, level: level, relativeLevel: relativeLevel }) } //[OK] win

    isAlwaysOnTop() { return this.alwaysOnTop } //[OK] win

    moveAbove(mediaSourceId) {
        throw 'Not implemented yet'
    }

    moveTop() {
        throw 'Not implemented yet'
    }

    center() { this.setAttrSync('center', true) } //[OK] win

    setPosition(x, y, animate) {
        throw 'Not implemented yet'
    }

    getPosition() {
        throw 'Not implemented yet'
    }

    setTitle(title) {
        throw 'Not implemented yet'
    }

    getTitle() {
        throw 'Not implemented yet'
    }

    setSheetOffset(offsetY, offsetX) {
        throw 'Not implemented yet'
    }

    flashFrame(flag) {
        throw 'Not implemented yet'
    }

    setSkipTaskbar(skip) {
        skipTaskbar = skip
    }

    setKiosk(flag) {
        throw 'Not implemented yet'
    }

    isKiosk() {
        throw 'Not implemented yet'
    }

    isTabletMode() {
        throw 'Not implemented yet'
    }

    getMediaSourceId() {
        throw 'Not implemented yet'
    }

    getNativeWindowHandle() {
        throw 'Not implemented yet'
    }

    hookWindowMessage(message, callback) {
        throw 'Not implemented yet'
    }

    isWindowMessageHooked(message) {
        throw 'Not implemented yet'
    }

    unhookWindowMessage(message) {
        throw 'Not implemented yet'
    }

    unhookAllWindowMessages() {
        throw 'Not implemented yet'
    }

    setRepresentedFilename(filename) {
        throw 'Not implemented yet'
    }

    getRepresentedFilename() {
        throw 'Not implemented yet'
    }

    setDocumentEdited(edited) {
        throw 'Not implemented yet'
    }

    isDocumentEdited() {
        throw 'Not implemented yet'
    }

    focusOnWebView() {
        throw 'Not implemented yet'
    }

    blurWebView() {
        throw 'Not implemented yet'
    }

    capturePage(rect, opts) {
        throw 'Not implemented yet'
    }

    loadURL(url, opts) {
        this.sync('loadURL', {url: url, opts: opts})
    }

    loadFile(filePath, options) {
        throw 'Not implemented yet'
    }

    reload() {
        throw 'Not implemented yet'
    }

    setMenu() {
        throw 'Not implemented yet'
    }

    removeMenu() {
        throw 'Not implemented yet'
    }

    setProgressBar(progress, options) {
        throw 'Not implemented yet'
    }

    setOverlayIcon(overlay, description) {
        throw 'Not implemented yet'
    }

    invalidateShadow() {
        throw 'Not implemented yet'
    }

    setHasShadow(hasShadow) {
        throw 'Not implemented yet'
    }

    hasShadow() {
        throw 'Not implemented yet'
    }

    setOpacity(val) { this.opacity = val }
    getOpacity() { return this.opacity }

    setShape(rects) {
        throw 'Not implemented yet'
    }

    setThumbarButtons(buttons) {
        throw 'Not implemented yet'
    }

    setThumbnailClip(region) {
        throw 'Not implemented yet'
    }

    setThumbnailToolTip(toolTip) {
        throw 'Not implemented yet'
    }

    setAppDetails(options) {
        throw 'Not implemented yet'
    }

    showDefinitionForSelection() {
        throw 'Not implemented yet'
    }

    setIcon(icon) {
        throw 'Not implemented yet'
    }

    setWindowButtonVisibility(visible) {
        throw 'Not implemented yet'
    }

    setAutoHideMenuBar(hide) {
        throw 'Not implemented yet'
    }

    isMenuBarAutoHide() {
        throw 'Not implemented yet'
    }

    setMenuBarVisibility(visible) {
        throw 'Not implemented yet'
    }

    isMenuBarVisible() {
        throw 'Not implemented yet'
    }

    setVisibleOnAllWorkspaces(visible, options) {
        throw 'Not implemented yet'
    }

    isVisibleOnAllWorkspaces() {
        throw 'Not implemented yet'
    }

    setIgnoreMouseEvents(ignore, options) {
        throw 'Not implemented yet'
    }

    setContentProtection(enable) {
        throw 'Not implemented yet'
    }

    setFocusable(focusable) {
        throw 'Not implemented yet'
    }

    isFocusable() {
        throw 'Not implemented yet'
    }

    setParentWindow(parent) {
        throw 'Not implemented yet'
    }

    getParentWindow() {
        throw 'Not implemented yet'
    }

    getChildWindows() {
        throw 'Not implemented yet'
    }

    setAutoHideCursor(autoHide) {
        throw 'Not implemented yet'
    }

    selectPreviousTab() {
        throw 'Not implemented yet'
    }

    selectNextTab() {
        throw 'Not implemented yet'
    }

    showAllTabs() {
        throw 'Not implemented yet'
    }

    mergeAllWindows() {
        throw 'Not implemented yet'
    }

    moveTabToNewWindow() {
        throw 'Not implemented yet'
    }

    toggleTabBar() {
        throw 'Not implemented yet'
    }

    addTabbedWindow(browserWindow) {
        throw 'Not implemented yet'
    }

    setVibrancy(type) {
        throw 'Not implemented yet'
    }

    setBackgroundMaterial(material) {
        throw 'Not implemented yet'
    }

    setWindowButtonPosition(position) {
        throw 'Not implemented yet'
    }

    getWindowButtonPosition() {
        throw 'Not implemented yet'
    }

    setTouchBar(touchBar) {
        throw 'Not implemented yet'
    }

    setTitleBarOverlay(options) {
        throw 'Not implemented yet'
    }

}


class ProtonJS_App extends SK_Module_Root {
    constructor(opt = {}) {
        super('proton')

        this.__moduleInstanceConfig.__target = 'app'
        
        this.listeners = {}

        this.readyPromise = new Promise(resolve => { this.readyPromise_Resolver = resolve })

        this.startMonitorReadyState()
    }

    startMonitorReadyState(){
        if (this.readyTimer) return

        this.readyTimer = setInterval(()=>{
            if (!sk_api.application) return
            if (!sk_api.application.isReady) return
            
            this.emit('ready', ()=>{})
            
            if (this.readyPromise_Resolver) this.readyPromise_Resolver()
            delete this.readyPromise_Resolver
            
            clearInterval(this.readyTimer)
            delete this.readyTimer
        }, 1)
    }
    

    emit(res, respondWith) {
        var handler = this['handle_' + res.eventID]
        if (handler) handler(res)

        var listenerCB = this.listeners[res.eventID]
        if (!listenerCB){
            respondWith({})
            return
        }

        var opt = {...res.data, ...{}}

        var preventables = [
            'before-quit',
            'will-quit',
            'open-file',
            'continue-activity',
            'will-continue-activity',
            'update-activity-state',
            'certificate-error',
            'select-client-certificate',
            'login'
        ]

        var responseObj = {}

        if (preventables.includes(res.eventID)){
            opt.preventDefault = ()=>{ responseObj.defaultPrevented = true }
        }


        listenerCB(opt)
        
        respondWith(responseObj)
    }

    on(eventID, cb) {
        var listener = this.listeners[eventID]
        if (!listener) this.listeners[eventID] = cb
    }

    off(eventID, callback) {
        delete this.listeners[eventID]
    }



    handle_ready(){
        this._isReady = true
        this.readyPromise_Resolver()
    }



    //=========================//




    quit(){
        this.sync('quit')
    }

    exit(exitCode = 0){
        this.sync('exist', {exitCode: exitCode})
    }
    
    relaunch(opts = {}){
        this.sync('relaunch', opts)
    }
    
    isReady(){
        return sk_api.application.isReady
    }
    
    whenReady(){
        if (this.isReady()){
            if (this.readyPromise_Resolver) this.readyPromise_Resolver()
            delete this.readyPromise_Resolver

            return new Promise(resolve => { resolve() })
        }

        return this.readyPromise
    }
    
    focus(opts = {}){
        this.sync('focus', opts)
    }
    
    hide(){
        this.sync('hide')
    }
    
    isHidden(){
        return this.sync('isHidden')
    }
    
    show(){
        this.sync('show')
    }
    
    setAppLogsPath(path){
        this.sync('setAppLogsPath', {path: path})
    }
    
    getAppPath(){
        return this.sync('getAppPath')
    }
    
    getPath(name){
        this.sync('getPath', {name: name})
    }
    
    getFileIcon(path, opts){
        return new Promise(async resolve => {
            var res = await this.async('getFileIcon', {path: path, opts: opts})
            //convert res to a NativeImage class
        })
    }
    
    setPath(name, path){
        this.sync('setPath', {name: name, path: path})
    }
    
    getVersion(){
        return sk_api.staticInfo.application.version
    }
    
    getName(){
        return this.appName || sk_api.staticInfo.application.name
    }
    
    setName(name){
        this.name = appName
    }
    
    getLocale(){
        return navigator.language
    }
    
    getLocaleCountryCode(){
        return getLocale().split('-')[1]
    }
    
    getSystemLocale(){
        return this.sync('getSystemLocale')
    }
    
    getPreferredSystemLanguages(){
        return this.sync('getPreferredSystemLanguages')
    }
    
    addRecentDocument(path){
        this.sync('setPath', {path: path})
    }
    
    clearRecentDocuments(){
        this.sync('clearRecentDocuments')
    }
    
    setAsDefaultProtocolClient(){
        this.sync('clearRecentDocuments')
    }
    
    removeAsDefaultProtocolClient(){
        throw 'Not implemented'
    }
    
    isDefaultProtocolClient(){
        throw 'Not implemented'
    }
    
    getApplicationNameForProtocol(){
        throw 'Not implemented'
    }
    
    getApplicationInfoForProtocol(){
        throw 'Not implemented yet'
    }
    
    setUserTasks(){
        throw 'Not implemented yet'
    }
    
    getJumpListSettings(){
        throw 'Not implemented yet'
    }
    
    setJumpList(){
        throw 'Not implemented yet'
    }
    
    requestSingleInstanceLock(){
        throw 'Not implemented yet'
    }
    
    hasSingleInstanceLock(){
        throw 'Not implemented yet'
    }
    
    releaseSingleInstanceLock(){
        throw 'Not implemented yet'
    }
    
    setUserActivity(){
        throw 'Not implemented yet'
    }
    
    getCurrentActivityType(){
        throw 'Not implemented yet'
    }
    
    invalidateCurrentActivity(){
        throw 'Not implemented yet'
    }
    
    resignCurrentActivity(){
        throw 'Not implemented yet'
    }
    
    updateCurrentActivity(){
        throw 'Not implemented yet'
    }
    
    setAppUserModelId(){
        throw 'Not implemented yet'
    }
    
    setActivationPolicy(){
        throw 'Not implemented yet'
    }
    
    importCertificate(){
        throw 'Not implemented yet'
    }
    
    configureHostResolver(){
        throw 'Not implemented yet'
    }
    
    disableHardwareAcceleration(){
        throw 'Not implemented yet'
    }
    
    disableDomainBlockingFor3DAPIs(){
        throw 'Not implemented yet'
    }
    
    getAppMetrics(){
        throw 'Not implemented yet'
    }
    
    getGPUFeatureStatus(){
        throw 'Not implemented yet'
    }
    
    getGPUInfo(){
        throw 'Not implemented yet'
    }
    
    setBadgeCount(){
        throw 'Not implemented yet'
    }
    
    getBadgeCount(){
        throw 'Not implemented yet'
    }
    
    isUnityRunning(){
        throw 'Not implemented yet'
    }
    
    getLoginItemSettings(){
        throw 'Not implemented yet'
    }
    
    setLoginItemSettings(){
        throw 'Not implemented yet'
    }
    
    isAccessibilitySupportEnabled(){
        throw 'Not implemented yet'
    }
    
    setAccessibilitySupportEnabled(){
        throw 'Not implemented yet'
    }
    
    showAboutPanel(){
        throw 'Not implemented yet'
    }
    
    setAboutPanelOptions(){
        throw 'Not implemented yet'
    }
    
    isEmojiPanelSupported(){
        throw 'Not implemented yet'
    }
    
    showEmojiPanel(){
        throw 'Not implemented yet'
    }
    
    startAccessingSecurityScopedResource(){
        throw 'Not implemented yet'
    }
    
    enableSandbox(){
        throw 'Not implemented yet'
    }
    
    isInApplicationsFolder(){
        throw 'Not implemented yet'
    }
    
    moveToApplicationsFolder(){
        throw 'Not implemented yet'
    }
    
    isSecureKeyboardEntryEnabled(){
        throw 'Not implemented yet'
    }
    
    setSecureKeyboardEntryEnabled(){
        throw 'Not implemented yet'
    }
    
    setProxy(){
        throw 'Not implemented yet'
    }
    
    resolveProxy(){
        throw 'Not implemented yet'
    }
    
    setClientCertRequestPasswordHandler(){
        throw 'Not implemented yet'
    }
}

sk_api.__protonjs.app = new ProtonJS_App()

module.exports = {
    BrowserWindow: BrowserWindow,
    app: sk_api.__protonjs.app
}
