console.log('sk:debugMngr')

class SK_Module_debugMngr extends SK_Module_Root {
    showDevTools(id = 'sb'){
        return this.async('showDevTools', {target: id})
    }
    
    log(obj){
        return this.async('log', obj)
    }
}


module.exports = new SK_Module_debugMngr('debugMngr')
