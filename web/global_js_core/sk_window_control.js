var proton = require('proton')
console.log(proton)
this.window = new proton.BrowserWindow({
    ...{/* <wnd_config> */},
    ...{shell: true}
})