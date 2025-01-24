console.log('sk:debugMngr')

class SK_Module_debugMngr extends SK_Module_Root {
    showDevTools(id){
        return this.async('showDevTools', {target: id})
    }
}


module.exports = new SK_Module_debugMngr('debugMngr')