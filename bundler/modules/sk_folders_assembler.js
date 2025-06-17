const fs = require('fs');
const path = require('path');


class Folder {
    constructor() {
        this.entries = []
    }


    addEntry(opt) {
        this.entries.push({
            idx: this.files.length,
            path: opt.path,
            modsysPath: opt.modsysPath,
            size: opt.size,
            filename: opt.filename,
            offset: this.currentOffset
        })
    }
}

global.Folder = Folder

module.exports = {
    forFolders: function(opt) {
        var folders = opt.folders

        var entries = []
        
        for (var i = 0; i < folders.length; i++) {
            var entry = folders[i]
            
            if (!entry.isFolder) continue

          
            var res = fs.readdirSync((entry.modsysPath ? modsys_root : soft_backend_root) + entry.path)
            
            var folderEntries = {
                folders: [],
                files: []
            }

            for (var u in res) {
                if (shouldIgnoreFile(res[u])) continue // Skip ignored files

                if (fs.statSync(path.join((entry.modsysPath ? modsys_root : soft_backend_root), entry.path, res[u])).isDirectory()){
                    folderEntries.folders.push(res[u])
                } else {
                    folderEntries.files.push(res[u])
                }
            }

            if (entry.modsysPath){
                var x = 0
            }
            
            entries.push({
                path: entry.path,
                modsysPath: entry.modsysPath,
                entries: folderEntries
            })
        }

        var cppEntries = []

        for (var i in entries) {
            var folder = entries[i]
            var refPath = (folder.modsysPath ? folder.modsysPath.substr(0, folder.modsysPath.length - 1) + folder.path: folder.path)
            if (folder.modsysPath && folder.path === '/') refPath = folder.modsysPath.substr(0, folder.modsysPath.length - 1)
            var cppEntry = `       {"${refPath}", new SK_SoftBackend_Bundle_Entry_Info(0, 0, 0, 0,0, true, "", "${folder.entries.folders.join(',')}", "${folder.entries.files.join(',')}")}`
            cppEntries.push(cppEntry)
        }
        
        return cppEntries
    }
};