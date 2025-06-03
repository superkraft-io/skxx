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
    /*listAllFolders(root){
        var entries = []

        function readDirRecursive(currentPath) {
            const files = fs.readdirSync(currentPath);

            for (let i = 0; i < files.length; i++) {
                const file = files[i];
                
                if (shouldIgnoreFile(file)) continue // Skip ignored files

                const fullPath = path.join(currentPath, file);

                const stat = fs.statSync(fullPath);

                const info = {
                    originalPath: fullPath,

                    isFolder: stat.isDirectory(),
                    path: fullPath.replace(dirPath, ''),
                    filename: file,
                    size: stat.size,
                    folders: [],
                    files: []
                }

                totalSize += info.size

                if (stat.isDirectory()) {
                    let folderEntries = fs.readdirSync(fullPath);
                    
                    for (let u in folderEntries) {
                        var entryName = folderEntries[u];
                        if (shouldIgnoreFile(entryName)) continue
                        var entryStats = fs.statSync(path.join(fullPath, entryName));
                        if (entryStats.isDirectory()) info.folders.push(entryName);
                        else info.files.push(entryName);
                    }

                    entries.push(info); // Add directory to the list
                    
                    readDirRecursive(fullPath); // Recurse into subdirectory
                }
            }
        }



        readDirRecursive(dirPath);

        return entries
    },
    */

    forFolders: function(opt) {
        var folders = opt.folders

        var entries = []
        
        for (var i = 0; i < folders.length; i++) {
            var entry = folders[i]
            
            if (!entry.isFolder) continue

          
            var res = fs.readdirSync(soft_backend_root + entry.path)
            
            var folderEntries = {
                folders: [],
                files: []
            }

            for (var u in res) {
                if (shouldIgnoreFile(res[u])) continue // Skip ignored files

                if (fs.statSync(path.join(soft_backend_root, entry.path, res[u])).isDirectory()){
                    folderEntries.folders.push(res[u])
                } else {
                    folderEntries.files.push(res[u])
                }
            }

            entries.push({path: entry.path, entries: folderEntries})
        }

        var cppEntries = []

        for (var i in entries) {
            var folder = entries[i]
            var cppEntry = `       {"${folder.path}", new SK_SoftBackend_Bundle_Entry_Info(0, 0, 0, 0,0, true, "", "${folder.entries.folders.join(',')}", "${folder.entries.files.join(',')}")}`
            cppEntries.push(cppEntry)
        }
        
        return cppEntries
    }
};