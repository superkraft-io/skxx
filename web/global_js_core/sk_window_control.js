var proton = require('proton')

this.window = new proton.BrowserWindow({
    ...{/* <wnd_config> */},
    ...{shell: true}
})