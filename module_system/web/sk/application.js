class SK_Application {
    constructor(opt) {
        this.isReady = false
        
        sk_api.ipc.on('sk:appEvent', (res, rW)=>{
            if (res.eventID === 'ready'){
                this.isReady = true
                if (this.onReady) this.onReady()
            }
            
            rW({})
        })
    }

    get mode() { return sk_api.staticInfo.application.mode }
    get name() { return sk_api.staticInfo.application.name }
    get version() { return sk_api.staticInfo.application.version }
}

var __sk_application = new SK_Application()

module.exports = __sk_application
