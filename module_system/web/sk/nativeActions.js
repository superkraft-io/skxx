console.log('sk_nativeActions.js')

class SK_Module_nativeActions extends SK_Module_Root {
    constructor(modulePath) {
        super(modulePath)

        for (var i in sk_api.nativeActions_definitions) {
            var id = sk_api.nativeActions_definitions[i]
            this.__createActionFunction(id)
        }
    }

    __createActionFunction(id) {
        this[id] = async opt => {
            return this.async(id, opt)
        }
    }
}

module.exports = new SK_Module_nativeActions('nativeActions')