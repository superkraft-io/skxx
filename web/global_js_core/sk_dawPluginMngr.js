console.log('sk_dawPluginMngr')

class sk_dawPluginMngr {
    constructor() {
        this.firstTimeRun = true

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
                    if (this.onData) this.onData({
                        ...metadata,
                        ...{
                            data: event.getBuffer()
                        }
                    })
                }
            })
        } catch (err) { }

        this.startReadMonitor()
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
