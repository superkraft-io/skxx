class SK_Module_process extends SK_Module_Root {
    on(eventName, cb){ window.addEventListener(eventName, cb) }
    off(eventName, cb){ window.removeEventListener(eventName, cb) }

    get env() { return this.sync('env') }

    abort(){
    }


    get allowedNodeEnvironmentFlags() { return }

    get arch() { return sk_api.staticInfo.machine.arch }

    get argv() { return sk_api.staticInfo.application.argv }
    get argv0() { return sk_api.staticInfo.application.argv0 }

    get channel() {
        return {
            ref: ()=>{

            },

            unref: ()=>{

            }
        }
    }

    chdir(directory) {
        console.log('chdir')
        try {
            var res = this.sync('chdir', directory)
            if (res.error) throw res.error
        } catch (err) {
            throw err
        }
    }
    get config() { }
    get connected() { }
    constrainedMemory() { }
    availableMemory() { }
    cpuUsage(previousValue) { }
    cwd() { }
    debugPort() { }
    disconnect() { }
    dlopen(module, filename, flags) { }
    emitWarning(warning, options, code, ctor) { }
    get execArgv() { }
    get execPath() { }
    exit(code) { }
    get exitCode() { }
    get finalization() {
        return {
            register: (ref, callback)=>{ },
            registerBeforeExit: (ref, callback)=>{ },
            unregister: (ref)=>{ }
        }
    }
    getActiveResourcesInfo() { }
    getBuiltinModule(id) { }
    getegid() { }
    geteuid() { }
    getgid() { }
    getgroups() { }
    getuid() { }
    hasUncaughtExceptionCaptureCallback() { }
    hrtime(time) {
        return {
            bigint() { }
        }
    }
    initgroups(user, extraGroup){ }
    kill(pid, signal){ }
    loadEnvFile(path){ }
    get mainModule(){ }
    memoryUsage(){
        return {
            rss(){

            }
        }
    }
    nextTick(callback){ }
    get noDeprecation(){ }
    get permission(){
        return {
            has(scope, reference) { }
        }
    }
    get pid() { }
    get platform() { return sk_api.staticInfo.machine.platform }
    get ppid(){ }
    get release(){ }
    get report(){
        return {
            compact: ()=>{ },
            directory: ()=>{ },
            filename: ()=>{ },
            getReport: (err)=>{ },
            reportOnFatalError: ()=>{ },
            reportOnSignal: ()=>{ },
            reportOnUncaughtException: ()=>{ },
            signal: ()=>{ },
   
            writeReport: (filename, err)=>{ }

        }
    }
    resourceUsage(){ }
    send(message, sendHandle, options, callback){ }
    setegid(id){ }
    seteuid(id){ }
    setgid(id){ }
    setgroups(groups){ }
    setuid(id){ }
    setSourceMapsEnabled(val){ }
    setUncaughtExceptionCaptureCallback(fn){

    }
    get sourceMapsEnabled() { }
    get stderr(){
        return {
            fd: ()=>{

            }
        }
    }
    get stdin(){
        return {
            fd: ()=>{

            }
        }
    }
    get stdout() {
        return {
            fd: ()=>{

            }
        }
    }
    get throwDeprecation(){ }
    get title(){ }
    get traceDeprecation(){ }
    umask(){ }
    umask(mask){ }
    uptime(){ }
    get version(){ }
    get versions(){ }
}

module.exports = new SK_Module_process('process')