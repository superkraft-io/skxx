
class SK_Module_BDFS extends SK_Module_Root {
    constructor(modulePath) {
        super(modulePath)
        
        this.promises = new SK_Module_BDFS_Promises(this)

        window.vfs = this
    }

    async init() {
        if (this.promises.init) this.promises.init()
    }
}

class SK_Module_BDFS_Promises {
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
        throw 'Cannot write to BDFS module'
    }

    readFile(path) {
        return new Promise(async (resolve, reject) => {
            try {
                var res = await this.parent.async('readFile', { path: path })
                var decodedData = atob(res.data)
                resolve(decodedData)
            } catch (err) {
                reject(err)
            }
        })
    }

    readdir(path, asObj) {
        return new Promise(async (resolve, reject) => {
            try {
                var res = await this.parent.async('readdir', { path: path })

                var list = []

                for (var i = 0; i < res.length; i++) {
                    list.push((!asObj ? res[i].name : res[i]))
                }

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
        throw 'Cannot write to BDFS module'
    }
}


module.exports = new SK_Module_BDFS('bdfs')