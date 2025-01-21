class SK_Module_os extends SK_Module_Root {
    get EOL() { return sk_api.staticInfo.machine.EOL }
    availableParallelism() { return this.sync('getCPUInfo').coreCount }
    arch() { return sk_api.staticInfo.machine.arch }
    get constants() { return '' }
    cpus() { return this.sync('getCPUInfo').cores }
    get devNull() { return sk_juce_api.staticInfo.machine.devNull }
    endianness() { return sk_juce_api.staticInfo.machine.endianess  }


    /* memory */

    meminfo() { return this.sync('getMemoryInfo') }
    totalmem() { return this.sync('getMemoryInfo').physical.total }
    freemem() { return this.sync('getMemoryInfo').physical.free }
    usedmem() { return sk_juce_api.fetch('getMemoryInfo').physical.used }



    uptime() {  return this.sync('getMachineTime').uptimem }


    getPriority(pid) {  /*IPC call*/ }
    setPriority(pid, priority) { /*IPC call*/ }


    /* paths */

    homedir() { return sk_api.staticInfo.machine.homedir }
    tmpdir() { return sk_api.staticInfo.machine.tmpdir }


    /* machine */
    hostname() { return sk_api.staticInfo.machine.hostname }

    loadavg() { /*IPC call*/ }

    /* operating system */
    version() { return sk_api.staticInfo.machine.version }
    platform() { return sk_api.staticInfo.machine.platform }
    release() { return sk_api.staticInfo.machine.release }
    machine() { return sk_api.staticInfo.machine.machine }
    type() { return sk_api.staticInfo.machine.type }



    /* network */
    networkInterfaces() { return this.sync('getNetworInfo').networkInterfaces }

    

    userInfo(options) { return this.sync('getUserInfo') }
}

module.exports = new SK_Module_os('os')