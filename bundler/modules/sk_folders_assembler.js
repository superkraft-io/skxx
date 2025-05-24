const fs = require('fs');
const path = require('path');


class Folder {
    constructor() {
        this.entries = []
    }


    addEntri(opt) {
        this.entries.push({
            idx: this.files.length,
            path: opt.path,
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

          
            var res = fs.readdirSync(soft_backend_root + entry.path)
            
            var folderEntries = []
            for (var u in res) {
                if (shouldIgnoreFile(res[u])) continue // Skip ignored files
                folderEntries.push(res[u])
            }

            entries.push({path: entry.path, entries: folderEntries})
        }

        var cppEntries = []

        for (var i in entries) {
            var folder = entries[i]
            var cppEntry = `       {"${folder.path}", new SK_SoftBackend_Bundle_Entry_Info(0, 0, 0, 0,0, true, "", "${folder.entries.join(',')}")}`
            cppEntries.push(cppEntry)
        }
        
        return cppEntries
    }
};