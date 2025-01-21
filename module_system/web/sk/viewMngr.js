class SK_Module_viewMngr extends SK_Module_Root {
    create(opt){
        return this.async('create', opt)
    }
}

module.exports = new SK_Module_viewMngr('viewMngr')