console.log('sk:web')

class SK_Module_web extends SK_Module_Root {
    async __call(func, payload = {}, onProgress){
        if (!payload.url) {
            throw '[sk:web] No URL has been set'
        }

        var callbackTimer = undefined
        var progressCallbackID = undefined

        if (onProgress) {
            var interval = payload.interval || 1000
            if (interval < 0)

            var cbRes = await this.async('sk.web', 'createProgressCallback')
            progressCallbackID = cbRes.progressCallbackID

            var again = true
            var callbackTimer = setInterval(async () => {
                if (again) return
                again = false
                progressRes = await this.async('sk.web', 'getProgress', { id: cbRes.progressCallbackID })
                onProgress(progressRes)
                again = true
            })
        }

        var res = undefined
        try {
            var defPayload = {
                url: '',
                headers: {},
                timeout: 7000,
                redirects: 5,
                mimeType: (func == 'get' ? 'text/html' : 'application/json'),
                body: (payload.body ? JSON.stringify(payload.body) : {})
            }

            defPayload = { ...defPayload, ...payload }



            var headersArr = []
            for (var headerName in defPayload.headers) {
                var headerValue = defPayload.headers[headerName]
                headersArr.push(headerName + ': ' + headerValue)
            }
            defPayload.headers = headersArr


            res = await this.async(func, {
                ...{ progressCallbackID: progressCallbackID },
                ...defPayload
            })

        } catch (err) {
            res = err
        }

        clearInterval(callbackTimer)

        return res
    }

    get(opt, onProgress){
        return this.__call('get', opt, onProgress)
    }

    post(opt, onProgress){
        return this.__call('post', opt, onProgress)
    }

    download(opt, onProgress) {
        //WIP
    }
}


module.exports = new SK_Module_web('web')