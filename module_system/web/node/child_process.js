class SK_Module_child_process extends SK_Module_Root {

    exec(command, options, callback){
        this.async('exec', {options: options }).then(res => {
            callback(res)
        })
    },

    execFile(file, args, options, callback){
        this.async('execFile', {args: args, options: options }).then(res => {
            callback(res)
        })
    },

    fork(file, args, option, callback){
        console.warning('node:child_process.fork() has not been implemented')
    },

    spawn(command, args, options){
        this.async('spawn', {args: args, options: options }).then(res => {
            
        })
    }
}

module.exports = new SK_Module_Process('node:child_process')