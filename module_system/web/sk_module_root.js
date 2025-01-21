class SK_Module_Root {
    constructor(modulePath = ''){
        this.modulePath = modulePath
    }

    sync(operation, payload){
        return SK_Module.syncOperation(this.modulePath, operation, payload)
    }

    async(operation, payload){
        return SK_Module.asyncOperation(this.modulePath, operation, payload)
    }
}