var fs = require('fs')
var path = require('path')

module.exports = class SK_NativeActions_Assembler {
    constructor(){
        this.headerPath = path.resolve(__dirname + '/../../../.sk/sk_project_nativeActions.hpp')
        this.template = fs.readFileSync(__dirname + '/templates/sk_project_nativeActions.hpp').toString()

        this.nativeActions = []
    }

    assemble(){
        var nativeActions_root = path.resolve(__dirname + '/../../../native_actions') + '/'
        try { fs.mkdirSync(nativeActions_root) } catch(err) {}


        var includeLines = []
        var listEntries = []

        var dirs = fs.readdirSync(nativeActions_root)
        for (var i in dirs){
            var filename = dirs[i]
            var actionName = filename.split('.')[0]
            var className = 'SK_NativeAction_' + actionName

            includeLines.push(`#include "../native_actions/${actionName}/${filename}.hpp"`)
            listEntries.push(`{"${actionName}", new ${className}()}`)
        }

        var outputFile = this.template
            .replace('//<includes>', includeLines.join('\n'))
            .replace('//<entries>', listEntries.join(',\n'))

        
        fs.writeFileSync(this.headerPath, outputFile)
    }
}