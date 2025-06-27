

//!!!!!    TODO: Also bundle all modsys web files, not just the soft backend files.    !!!!


const fs = require('fs')
const path = require('path')

var lister = require('./modules/sk_file_lister')
var grouper = require('./modules/sk_files_grouper')
var folders_assembler = require('./modules/sk_folders_assembler')

global.modsys_root = path.resolve(__dirname, '../module_system/web/')
global.modsys_roots = {
    node: modsys_root + '/node/',
    npm: modsys_root + '/npm/',
    sk: modsys_root + '/sk/'
}

global.web_core_root = path.resolve(__dirname, '../web/')
global.web_core = {
    global_js_core: web_core_root + '/global_js_core/',
    soft_backend: web_core_root + '/soft_backend/'
}

global.soft_backend_root =  path.resolve(__dirname, '../../project')

global.bundleRoot = path.resolve(__dirname, '../../sk_soft_backend_bundle/')

global.bundleDeepRoot = path.resolve(__dirname, '../../sk_soft_backend_bundle/deep/')
global.deepGroupsRoot = path.resolve(__dirname, '../../sk_soft_backend_bundle/deep/groups/')

global.bundleShallowRoot = path.resolve(__dirname, '../../sk_soft_backend_bundle/shallow/')
global.shallowGroupsRoot = path.resolve(__dirname, '../../sk_soft_backend_bundle/shallow/groups/')
global.shallowGroupsDataRoot = path.resolve(__dirname, '../../sk_soft_backend_bundle/shallow/groups/data/')


fs.rmdirSync(bundleRoot, { recursive: true, force: true })

fs.mkdirSync(bundleDeepRoot, { recursive: true })
fs.mkdirSync(deepGroupsRoot, { recursive: true })

fs.mkdirSync(bundleShallowRoot, { recursive: true })
fs.mkdirSync(shallowGroupsRoot, { recursive: true })
fs.mkdirSync(shallowGroupsDataRoot, { recursive: true })

fs.copyFileSync(__dirname + '/templates/sk_soft_backend_bundle_group_root_template.h', path.resolve(__dirname, '../../sk_soft_backend_bundle/sk_soft_backend_bundle_group_root.h'))



var modsysEntries = []
var webcoreEntries = []
/*for (var key in modsys_roots){
    var modulesPath = modsys_roots[key]
    modsysEntries = [...modsysEntries, ...lister.listFiles(modulesPath, 'sk:modsys/')]
}*/

webcoreEntries = lister.listFiles(web_core_root, 'sk:webcore/')
modsysEntries = lister.listFiles(modsys_root, 'sk:modsys/')

var allEntries = [
    ...webcoreEntries,
    ...modsysEntries,
    ...lister.listFiles(soft_backend_root)
]



var groupRes = grouper.forFiles({
    files: allEntries,
    groupSize: 0.3, // MB
})

var foldersRes = folders_assembler.forFolders({
    folders: allEntries
})

var libraryTemplatePath =  __dirname + '/templates/sk_soft_backend_bundle_library_template.h'
var libraryTemplate = fs.readFileSync(libraryTemplatePath).toString()
    .replace('<!deep_group_includes!>', groupRes.includesDef.deep)
    .replace('<!shallow_group_includes!>', groupRes.includesDef.shallow)
    .replace('<!groups!>', groupRes.groupsDefs + '\n')
    .replace('<!file_entries!>', groupRes.entriesDefs + '\n')
    .replace('<!folder_entries!>', foldersRes.join(',\n') + '\n')

var libraryPath = path.resolve(__dirname, '../../sk_soft_backend_bundle/sk_soft_backend_bundle_library.h')
fs.writeFileSync(libraryPath, libraryTemplate)


console.log(`Done!`)