class SK_Module_electron {
    get shell() {
        return {
            showItemInFolder: (fullPath)=>{
                return this.async('showItemInFolder', { path: fullPath })
            },

            openPath: (path) => {
                return this.async('openPath', { path: path })
            },

            openExternal: (url, options) => {
                return this.async('openExternal', { url: url, options: options })
            },

            trashItem: (path) => {
                return this.async('trashItem', { path: path })
            },

            beep: () => {
                return this.async('beep')
            },

            writeShortcutLink: (shortcutPath, operation, options) => {
                return this.async('writeShortcutLink', { path: shortcutPath, operation: operation, options: options })
            },

            readShortcutLink: (shortcutPath) => {
                return this.async('readShortcutLink', { path: shortcutPath })
            }
        }
    }
}

module.exports = new SK_Module_electron('electron')