console.log('SK IPC')
class SK_IPC {
    constructor(opt) {
        this.msg_id = 0
        this.awaitList = {}
        this.listeners = {}

        this.sender_id = opt.sender_id

        this.on('sk.sb.forwardedPacket', async data => {
            return await this.handleRequest(data, true)
        })

        this.on('sk.sb.ipc', async res => {
            return await sk_api.ipc.handleRequest({ event_id: res.cmd, data: res }, true)
        })
    }

    sendToBE(event_id, data = {}, type = "request", overridePacketInfo = {}) {
        if (!event_id) throw "[SK IPC.sendToBE] Event ID cannot be empty"

        this.msg_id++
        var msg_id = this.msg_id

        //send to backend
        var req = {
            ...{
                isSK_IPC_call: true,
                msg_id: msg_id.toString(),
                type: type,
                sender: (window.sk && window.sk.id ? window.sk.id : undefined) || this.sender_id,
                target: "sk:sb",
                event_id: event_id,
                data: data
            },

            ...overridePacketInfo
        }

        __SK_IPC_Send(req)
        
        return msg_id
    }

    request(event_id, data = {}, timeout = 10000, overridePacketInfo = {}) {
        return new Promise((resolve, reject) => {
            var msg_id = this.sendToBE(event_id, data, "request", overridePacketInfo)

            var timeoutTimer = setTimeout(() => {
                resolve()
                return

                var err = `[SK IPC.send] Send event ${event_id} timed out`
                console.error(err)
                console.log('----- END -----')
                reject(err)
            }, timeout)

            this.awaitList[msg_id] = {
                resolve: resolve,
                reject: reject,
                timeoutTimer: timeoutTimer
            }
        })
    }

    message(data) {
        this.sendToBE("SK_IPC_Message", data, "message")
    }

    on(event_id, cb, once) {
        if (!event_id) throw "[SK IPC.on] Event ID cannot be empty"

        if (this.listeners[event_id]) console.warning(`[IPC.on] And event ID named "${event_id}" has already previously been `)

        var opts = { callback: cb }

        if (once) opts.once = true

        this.listeners[event_id] = opts
    }

    once(event_id, cb) {
        this.on(event_id, cb, true)
    }




    handleIncoming(res) {
        if (res.type === "response") this.handleResponse(res)
        else if (res.type === "request") this.handleRequest(res)
        else if (res.type === "message") this.handleMessage(res)
        else if (res.type === "ipcTestResponse") this.handleIPCTestResponse(res)
        else throw `[SK IPC.handleIncoming] Invalid incoming IPC message type`
    }

    handleResponse(res) {
        try {
            res.data = JSON.parse(res.data)
            res.data_type = typeof res.data
        } catch (err) {
            res.data_type = "unknown"
        }

        var awaiter = this.awaitList[res.msg_id]

        delete this.awaitList[res.msg_id]

        if (!awaiter) throw `[SK IPC.handleResponse] Invalid IPC response for event ID "${res.event_id}" with message ID "${res.msg_id}"`

        clearTimeout(awaiter.timeoutTimer)

        if (res.data && res.data.error) {
            awaiter.reject(res.data)
            return
        }

        awaiter.resolve(res.data)
    }

    //A request expects a response
    async handleRequest(res, isForwardedPacket) {
        var resJSON = res
        try {
            resJSON = JSON.parse(res)
        } catch (err) {
            resJSON = res
        }

        var listener = this.listeners[resJSON.event_id]

    
        var responseData = {
            error: "unknown_listener",
            message: "A listener with the event ID \"" + resJSON.event_id + "\" does not exist."
        }


        var responseData_A = {}
        var responseData_B = {}
        if (listener) {
            if (listener.callback.constructor.name == "AsyncFunction") responseData_A = await listener.callback(resJSON.data, res => { responseData_B = res })
            else responseData_A = listener.callback(resJSON.data, res => { responseData_B = res })
        }

        var responseData = responseData_A || responseData_B

        if (!isForwardedPacket) {
            this.sendToBE(
                resJSON.event_id,
                responseData,
                "response",
                {
                    msg_id: resJSON.msg_id,
                    sender: resJSON.target,
                    target: resJSON.sender
                }
            )
        } else {
            return responseData
        }
    }

    //A message doess not expect a response. The backend has simply sent a message that the app can handle as it wishes.
    handleMessage(res) {
        if (this.onMessage) this.onMessage(res.data)
    }

    async test(calls = 5000) {
        var startMS = Date.now()

        var promises = []
        for (var i = 0; i < calls; i++) promises.push(sk_api.ipc.request('valid_event_id', { key: 'value' }))

        await Promise.all(promises)

        var endMS = Date.now()

        var elapsed = endMS - startMS

        console.log('IPC test completed in ' + elapsed + 'ms')
    }

    async testSingle(type = 'nlohmann') {
        return new Promise((resolve, reject) => {
            this.msg_id++
            var msg_id = this.msg_id

            this.awaitList[msg_id] = {
                resolve: resolve,
                reject: reject
            }


            var msg = `@${type}${msg_id}`
            window.chrome.webview.postMessage(msg);
        })
    }

    handleIPCTestResponse(res) {
        //console.log('handling ' + res.msg_id)
        var awaiter = this.awaitList[res.msg_id]
        delete this.awaitList[res.msg_id]
        awaiter.resolve(res.data)
    }

    async testIPC(type = 'nlohmann', calls = 100) {
        var startMS = Date.now()

        var promises = []
        for (var i = 0; i < calls; i++) promises.push(this.testSingle(type))

        await Promise.all(promises)

        var endMS = Date.now()

        var elapsed = endMS - startMS

        console.log('IPC test completed in ' + elapsed + 'ms')
    }
}

window.SK_IPC = SK_IPC;
