console.log('sk_debug_mode.js')

/*
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
    
    log: (...args)=>{
        sk_console.log(...args)
        console.__printToBackend('log', JSON.stringify(args))
    },
        
    warning: (...args)=>{
        sk_console.warning(...args)
        console.__printToBackend('warning', JSON.stringify(args))
    },
        
    error: (...args)=>{
        sk_console.error(...args)
        console.__printToBackend('error', JSON.stringify(args))
    },
       
    info: (...args)=>{
        sk_console.info(...args)
        console.__printToBackend('info', JSON.stringify(args))
    }
}
*/
if (sk_api.staticInfo.application.mode === 'debug') console.printToBackend = true
