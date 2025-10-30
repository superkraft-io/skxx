console.log('sk_dawPluginMngr')

class sk_dawPluginMngr {
    constructor(parent) {
        this.parent = parent

        this.firstTimeRun = true

        this.__sharedBufferQueue = {}
        
        this.components = {}

        this.parameters = '<sk_plugin_parameters>'
        this.parametersIdxByID = {}
        for (var i = 0; i < this.parameters.length; i++) this.parametersIdxByID[this.parameters[i].id] = i
        
            
        //On windows, we use the shared buffer to send the parameters to the plugin
        try {
            window.chrome.webview.addEventListener('sharedbufferreceived', (event) => {
                const metadata = event.additionalData;
                if (metadata?.id === 'pluginParamUpdate') {
                    const arrayBuffer = event.getBuffer();
                    const floatArray = new Float32Array(arrayBuffer);
                    for (var i = 0; i < this.parameters.length; i++){
                        var newVal = floatArray[i]
                        this.updateParameter(i, newVal)
                    }
                    chrome.webview.releaseBuffer(arrayBuffer);
                }
                else {
                    notifySharedBufferReceived(metadata, event.getBuffer())
                }
            })
        } catch (err) { }

        if (this.parent.staticInfo.os === 'macos') this.sendSharedBufferReadyState()

        this.startReadMonitor()

        this.amount = 0
        this.lowest = 0
        this.highest = 0

        setInterval(() => {
            this.lowest = 0
            this.highest = 0
            this.amount = 0
        }, 5000)
    }

    logTime(time){
        this.amount++

        if (time < this.lowest || this.lowest === 0) this.lowest = time
        if (time > this.highest) this.highest = time

        console.log(`Lowest: ${this.lowest} ms, Highest: ${this.highest} ms, Amount: ${this.amount}, Last: ${time} ms`)
    }

    sendSharedBufferReadyState(){
        fetch(sk_api.baseURL + '/__sk_sharedBuffer?setReadyState=true', { method: 'GET' })
        .then(response => {
            if (!response.ok) throw new Error(`HTTP error ${response.status}`);
            return response.arrayBuffer()
        })
        .then(data => {
            console.lod('Shared buffer ready state set');
        })
        .catch(error => {
            console.error('Failed setting sharedBuffer ready state');
        });
    }

    async fetchQueuedBuffer(uuid, metadata){ //Used in MacOS only

        this.__sharedBufferQueue[uuid] = metadata

        fetch(sk_api.baseURL + '/__sk_sharedBuffer?get=true&uuid=' + uuid, { method: 'GET' })
        .then(response => {
            if (!response.ok) throw new Error(`HTTP error ${response.status}`);
            return response.arrayBuffer()
        })
        .then(data => {
            var _metadata = this.__sharedBufferQueue[uuid]
            delete this.__sharedBufferQueue[uuid] 
            this.notifySharedBufferReceived(_metadata, data)
        })
        .catch(error => {
            console.error('Failed getting sharedBuffer');
        });
    }

    notifySharedBufferReceived(metadata, data){
        if (this.onData){
            this.onData({
                ...metadata,
                ...{ data: data }
            })
        }
    }

    parameterByID(id){
        return {...this.parameters[this.parametersIdxByID[id]], ...{component: this.components[id]}}
    }

    updateParameter(idx, value){
        //This function will be called directly from the native code on Apple platforms since Apple does not support shared buffers
        //On Windows, we use the shared buffer to send the parameters to the plugin

        if (value === this.parameters[idx].value) return
        this.parameters[idx].value = value
    }

    add(dawPluginParamID, component) {
        this.components[dawPluginParamID] = component

        component.dawPluginParamInfo = this.parameterByID(dawPluginParamID)

        component.__dawPluginWriteParamValue = async val => {
            var touchUpElapsed = Date.now() - component.dawPluginParamIsTouchingUpTime
            var canWrite = component.dawPluginParamIsTouching || (touchUpElapsed < 50)
            if (canWrite) await sk.nativeActions.handlePluginParamMouseEvent({dawPluginParamID: component.__dawPluginParamID, event: 'write', value: val})
        }

        component.element.addEventListener('contextmenu', async _e => {
            if (component.disabled) return

            _e.stopPropagation()
            _e.preventDefault()

            if (_e.button === 2) {
                var x = _e.clientX
                var y = _e.clientY

                if (sk_api.staticInfo.machine.os === 'windows'){
                    x *= window.devicePixelRatio
                    y *= window.devicePixelRatio
                }

                try {
                    await sk.nativeActions.handlePluginParamMouseEvent({
                        dawPluginParamID: component.__dawPluginParamID,
                        event: 'contextmenu',
                        left: x,
                        top: y,
                    })
                } catch (err) {
                    if (err.error) {
                        if (err.error === 'standalone_runtime') target.hint({ text: '⚠️ Native DAW context menus only work when the plugin is running inside a DAW', instaShow: true, position: 'top center' })
                        component.hint({ text: '' })
                    }
                }
            }
        })
    }

    startReadMonitor() {
        var step = async _ts => {
            for (var id in this.components) {
                var component = this.components[id]
                if (!component.dawPluginParamIsTouching || this.firstTimeRun){
                    component.value = this.parameterByID(id).value
                }
            }

            delete this.firstTimeRun
            window.requestAnimationFrame(step)
        }

        window.requestAnimationFrame(step)
    }
}
