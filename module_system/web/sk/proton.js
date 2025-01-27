console.log('sk:proton')


class ProtonWindow {
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

    }

    get ids() {

    }

    get tabbingIdentifier() {

    }


    set autoHideMenuBar(val) {

    }

    get autoHideMenuBar() {

    }


    set simpleFullScreen(val) {

    }
    get simpleFullScreen() {

    }


    set fullScreen(val) {

    }
    get fullScreen() {

    }


    set focusable(val) {

    }
    get focusable() {

    }


    set visibleOnAllWorkspaces(val) {

    }

    get visibleOnAllWorkspaces() {
        if (sk_api.staticInfo.machine.os == "win") return false //always returns false on windows

        //macos result here
    }


    set shadow(val) {

    }
    get shadow() {

    }


    set menuBarVisible(val) {

    }

    get menuBarVisible() {

    }


    set kiosk(val) {

    }
    get kiosk() {

    }


    set documentEdited(val) {

    }
    get documentEdited() {

    }


    set representedFilenames(val) {

    }

    get representedFilenames() {

    }


    set title(val) {

    }
    get title() {

    }

    set minimizable(val) {

    }

    get minimizable() {

    }


    set maximizable(val) {

    }
    get maximizable() {

    }


    set fullScreenable(val) {

    }

    get fullScreenable() {

    }


    set resizable(val) {

    }

    get resizable() {

    }


    set closable(val) {

    }
    get closable() {

    }


    get movable(val) {

    }
    get movable() {

    }

    set excludedFromShownWindowsMenu(val) {

    }
    get excludedFromShownWindowsMenu() {

    }

    set accessibleTitle(val) {

    }
    get accessibleTitle() {

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

    setAlwaysOnTop(flag, level, relativeLevel) {

    }

    isAlwaysOnTop() {

    }

    moveAbove(mediaSourceId) {

    }

    moveTop() {

    }

    center() {

    }

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

    setIgnoreMouseEvents(ignore[, options]) {

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

class Proton extends SK_Module_Root {
    
}


module.exports = new SK_Module_proton('proton')