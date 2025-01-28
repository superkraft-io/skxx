class SK_Module_Root {
    constructor(modulePath = '', config = {}){
        this.modulePath = modulePath
        this.__moduleInstanceConfig = config
    }

    sync(operation, payload){
        return SK_Module.syncOperation(this.modulePath, operation, { ...payload, ...{__moduleInstanceConfig: this.__moduleInstanceConfig} })
    }

    async(operation, payload){
        return SK_Module.asyncOperation(this.modulePath, operation, { ...payload, ...{ __moduleInstanceConfig: this.__moduleInstanceConfig } })
    }
}