/*function sleep(delay = 1000) {
    return new Promise(resolve => {
        setTimeout(() => {
            resolve()
        }, delay)
    })
}


var __start = async () => {
    await sleep(10000)
*/

    
    window.global = window
    window.sk_appType = 'skxx'

    window.onBreak = () => {
        var x = 0
    }

    sk_api.setID('sk.sb')

    var start_app = async () => {
        window.global = window

        await sk_api.initModules()



        window.process = require('node:process')
    
        try {
            window.appMain = new (require('/main.js'))()
        } catch(err) {
        
        }

        var opt = {
            type: window.sk_appType,
            root: '',

            projectRoot: '/sk_project',
            postsRoot: '/sk_posts/',
            templates: '/sk_templates/',
            globalActions: '/sk_globalActions/',
            globalFrontend: '/sk_globalFrontend/',

            nativeActions: __dirname + '/sk_nativeActions/',


            database: {},
            auth: {},

            l10n: {
                listCountries: () => { return [] },
                getForCountry: country => {
                    return {}
                }
            },

            onAppReady: async () => {

            },

            onPreStart: () => {
                try {
                    if (appMain.preSKStart) appMain.preSKStart()
                } catch (err) {
                    if (err.indexOf && err.indexOf('Could not fetch module') > -1) {
                        console.warn('Could not load the project main.js file. Create a file named main.js inside the "sk_project" folder.')
                    } else {
                        throw err
                    }
                }
            },

            onReady: async () => {
                if (appMain.postSKInit) appMain.postSKInit(sk)                
            }
        }



        if (appMain && appMain.preSKInit) appMain.preSKInit(opt)

        var __superkraft = require('/superkraft/sk_superkraft.js')
    
        var sk_start = ()=>{
            var sk = new __superkraft(opt)
        }
    
        window.sk_start = sk_start
    
        //sk_start()
    }




    start_app()

//}

//__start()