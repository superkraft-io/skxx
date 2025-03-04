console.log('sk_debug_mode.js')


window.sk_console = window.console


window.console = {
    printToBackend: false,
    __printToBackend(type, obj = ''){
        if (!console.printToBackend) return
            
        sk_api.ipc.message({
            isSKDebugCall: true,
            action: 'dbgLog',
            type: type,
            data: obj
        }, {target: 'sk:viewIPC'})
    },
    
    log: obj => {
        sk_console.log(obj)
        console.__printToBackend('log', obj)
    },
        
    warning: obj => {
        sk_console.warning(obj)
        console.__printToBackend('warning', obj)
    },
        
    error: obj => {
        sk_console.error(obj)
        console.__printToBackend('error', obj)
    }
}

if (sk_api.staticInfo.application.mode === 'debug') console.printToBackend = true
