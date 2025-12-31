class SK_Module_process extends SK_Module_Root {
    // --- Event Handling ---
    on(eventName, cb) { window.addEventListener(eventName, cb) }
    off(eventName, cb) { window.removeEventListener(eventName, cb) }

    // --- Dynamic Properties (Getters) ---
    
    /** @returns {Object} All environment variables */
    get env() { 
        return this.sync('process.env'); 
    }

    /** @returns {number} The current process ID */
    get pid() { 
        const res = this.sync('process.pid');
        return res ? res.pid : null;
    }

    /** @returns {number} The parent process ID */
    get ppid() {
        const res = this.sync('process.ppid');
        return res ? res.ppid : null;
    }

    /** @returns {string} The operating system platform (win32, linux, darwin) */
    get platform() { 
        // Preference: Static info if available, otherwise bridge to OS
        return sk_api.staticInfo?.machine?.platform || this.sync('process.platform').platform; 
    }

    /** @returns {string} The CPU architecture (x64, arm64, etc) */
    get arch() { 
        return sk_api.staticInfo?.machine?.arch || this.sync('process.arch').arch; 
    }

    /** @returns {string} The engine version string */
    get version() {
        return this.sync('process.version').version;
    }

    /** @returns {Object} Versions of various components */
    get versions() {
        return this.sync('process.versions');
    }

    // --- Methods ---

    /** @returns {string} The current working directory */
    cwd() { 
        const res = this.sync('process.cwd');
        return res ? res.cwd : '';
    }

    /** @param {string} directory - The path to change to */
    chdir(directory) {
        const res = this.sync('process.chdir', { directory });
        if (res && res.error) throw new Error(res.error);
        return res;
    }

    /** @returns {number} System uptime in seconds */
    uptime() {
        const res = this.sync('process.uptime');
        return res ? res.uptime : 0;
    }

    /** @returns {Object} Memory usage metrics in bytes */
    memoryUsage() {
        const res = this.sync('process.memoryUsage');
        return {
            rss: res.rss || 0,
            heapTotal: res.heapTotal || 0,
            heapUsed: 0,
            external: 0,
            arrayBuffers: 0
        };
    }

    /** @returns {Object} User and System CPU time in microseconds */
    cpuUsage(previousValue) {
        const usage = this.sync('process.cpuUsage');
        if (previousValue) {
            return {
                user: usage.user - previousValue.user,
                system: usage.system - previousValue.system
            };
        }
        return usage;
    }

    /** @returns {Array} High-resolution time [seconds, nanoseconds] */
    hrtime(time) {
        const res = this.sync('process.hrtime');
        if (time) {
            // If previous hrtime passed, return delta
            const s = res[0] - time[0];
            let ns = res[1] - time[1];
            if (ns < 0) { ns += 1e9; s -= 1; }
            return [s, ns];
        }
        return res;
    }

    /** @param {number} code - The exit status code */
    exit(code = 0) { 
        this.sync('process.exit', { code }); 
    }

    /** @param {number} pid - Process ID to kill. @param {string} signal - e.g. 'SIGTERM' */
    kill(pid, signal = 'SIGTERM') {
        this.sync('process.kill', { pid, signal });
    }

    /** Gets or sets the process umask */
    umask(mask) {
        const payload = mask !== undefined ? { mask: mask.toString(8) } : {};
        const res = this.sync('process.umask', payload);
        return res ? res.value : null;
    }

    // --- Process ID Setters/Getters (POSIX) ---
    getuid() { return this.sync('process.ids').uid; }
    getgid() { return this.sync('process.ids').gid; }
    geteuid() { return this.sync('process.ids').euid; }
    getegid() { return this.sync('process.ids').egid; }

    // --- Static Property Fallbacks ---
    get argv() { return sk_api.staticInfo?.application?.argv || []; }
    get argv0() { return sk_api.staticInfo?.application?.argv0 || ""; }
    get execPath() { return sk_api.staticInfo?.application?.execPath || ""; }

    // --- Stubs for complete API compatibility ---
    abort() { this.exit(1); }
    nextTick(callback) { setTimeout(callback, 0); }
    
    get stdout() { return { fd: 1, write: (str) => console.log(str) }; }
    get stderr() { return { fd: 2, write: (str) => console.error(str) }; }
    get stdin() { return { fd: 0 }; }
}

module.exports = new SK_Module_process('process');