
class SK_Module_VFS extends SK_Module_Root {
    constructor(opt) {
        this.sk = opt.sk
        this.promises = new SK_Module_VFS_Promises(this)

        window.vfs = this
    }

    async init() {
        if (this.promises.init) this.promises.init()
    }
}

class SK_Module_VFS_Promises {
    constructor(parent) {
        this.parent = parent
        this.sk = parent.sk
    }

    init() {
    }

    access(path) {
        return this.parent.async('access', { path: path })
    }

    stat(path) {
        return new Promise(async (resolve, reject) => {
            try {
                var info = await this.parent.async('stat', { path: path })
                info.isDirectory = () => { return info.type === 'dir' }
                resolve(info)
            } catch (err) {
                reject(err)
            }
        })
    }

    writeFile(path, data) {
        console.log('writeFile')
        return this.parent.async('writeFile', { path: path, data: { fileData: data } })
    }

    readFile(path) {
        console.log('readFile')
        return this.parent.async('readFile', { path: path })
    }

    readdir(path, asObj) {
        return new Promise(async (resolve, reject) => {
            try {
                var res = await this.parent.async('readdir', { path: path })
                var list = []
                for (var i in res) list.push((!asObj ? res[i].name : res[i]))
                resolve(list)
            } catch (err) {
                reject(err)
            }
        })
    }

    async readJSON(path) {
        console.log('readJSON')
        return JSON.parse(await this.parent.async('readJSON', { path: path }))
    }

    async writeJSON(path, data) {
        console.log('writeJSON')
        return JSON.parse(await this.parent.async('writeJSON', { path: path, data: JSON.stringify(data) }))
    }
}


module.exports = new SK_Module_VFS('vfs')