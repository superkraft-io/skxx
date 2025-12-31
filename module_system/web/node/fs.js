class SK_Module_fs extends SK_Module_Root {
    accessSync(path) {
        var res = this.sync('access', { path: path })
        return res
    }

    statSync(path) {
        try {
            var info = this.sync('stat', { path: path })
            info.isDirectory = () => { return info.type === 'dir' }
            return info
        } catch (err) {
            throw err
        }
    }

    writeFileSync(path, data) {
        return this.sync('writeFile', { path: path, data: data })
    }

    readFileSync(path) {
        try {
            var res = this.sync('readFile', { path: path })

            return atob(res.data)
        } catch (err) {
            throw err
        }
    }

    readdirSync(path, asObj) {
        try {
            var res = this.sync('readdir', { path: path })

            var list = []

            for (var i = 0; i < res.length; i++) {
                list.push((!asObj ? res[i].name : res[i]))
            }

            return list
        } catch (err) {
            throw err
        }
    }

    unlinkSync(path) {
        try {
            var res = this.sync('unlink', { path: path })
            if (res.error) throw res.err
        } catch (err) {
            throw err
        }
    }

    rmSync(path, options = {}){
        var _this = this

        var defOpts = {
            force: false,
            maxRetries: 0,
            recursive: false,
            retryDelay: 100
        }

        defOpts = { ...defOpts, ...options }

        var cancel = false
        function removeDirectoryRecursive(directoryPath) {
            if (cancel) return

            const files = _this.readdirSync(directoryPath);

            if (files.length === 0) {
                _this.unlinkSync(directoryPath)
                return
            }

            // Iterate through each file/directory
            for (const file of files) {
                var filePath = directoryPath + '/' + file;
                filePath = filePath.split('\\').join('/')
                filePath = filePath.split('//').join('/')

                if (_this.statSync(filePath).isDirectory()) {
                    removeDirectoryRecursive(filePath)
                    if (cancel) return
                } else {
                    var fileFailed = true
                    for (var attempt = 0; attempt <= defOpts.maxRetries; attempt++) {
                        try {
                            this.unlinkSync(filePath)
                            fileFailed = false
                        } catch (err) {
                        }
                    }

                    if (fileFailed) {
                        cancel = true
                        break
                    }
                }
            }

            if (cancel) return

            // After all files and subdirectories are removed, remove the empty directory
           

            removeDirectoryRecursive(directoryPath)
        }


        if (this.statSync(path).isDirectory()) removeDirectoryRecursive(path)
        else this.unlinkSync(path)
    }

    mkdirSync(path, options) {
        var defOpts = {
            recursive: false,
            mode: '0x777'
        }

        defOpts = { ...defOpts, ...options }


        try {
            var res = this.sync('mkdir', { ...{ path: path }, ...defOpts })
            if (res.error) throw res.err
        } catch (err) {
            throw err
        }
    }


    get promises() {
        var _this = this

        return {
            async access(path) {
                var res = await _this.async('access', { path: path })
                return res
            },

            stat(path) {
                return new Promise(async (resolve, reject) => {
                    try {
                        var info = await _this.async('stat', { path: path })
                        info.isDirectory = () => { return info.type === 'dir' }
                        resolve(info)
                    } catch (err) {
                        reject(err)
                    }
                })
            },

            writeFile(path, data) {
                return _this.async('writeFile', { path: path, data: data })
            },

            readFile(path) {
                return new Promise(async (resolve, reject) => {
                    try {
                        var res = await _this.async('readFile', { path: path })
                        var decodedData = atob(res.data)
                        resolve(decodedData)
                    } catch (err) {
                        reject(err)
                    }
                })
            },

            readdir(path, asObj) {
                return new Promise(async (resolve, reject) => {
                    try {
                        var res = await _this.async('readdir', { path: path })

                        var list = []

                        for (var i = 0; i < res.length; i++) {
                            list.push((!asObj ? res[i].name : res[i]))
                        }

                        resolve(list)
                    } catch (err) {
                        reject(err)
                    }
                })
            },

            rm(path, options) {
                return new Promise(async (resolve, reject) => {
                    try {
                        var res = this.rm(path, options)
                        if (res.error) return reject(res.err)
                        resolve()
                    } catch (err) {
                        reject(err)
                    }
                })
            },

            mkdir(path, options) {
                return new Promise(async (resolve, reject) => {
                    try {
                        var res = this.mkdir(path, options)
                        if (res.error) return reject(res.err)
                        resolve()
                    } catch (err) {
                        reject(err)
                    }
                })
            },
        }
    }
}

module.exports = new SK_Module_fs('fs')