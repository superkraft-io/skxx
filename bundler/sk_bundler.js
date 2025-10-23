global.sk = {
    file: 'sk_bundler.js',
    path: __dirname
}

const fs = require('fs')
const path = require('path')
var utils = require('./modules/sk_utils.js')

utils.printRuntimeUser()

if (utils.getOS() === 'unknown'){
    utils.reportError({msg: '[SK++ Bundler] Unsupported OS. Only Windows, MacOS and Linux are supported.'})
}

const { argv } = require('process')

var args = utils.parseArgs(argv.slice(2))
global.sk = {
    bundle_mode: args.bundle_mode //none, shallow, deep
}

var lister = require('./modules/sk_file_lister')
var grouper = require('./modules/sk_files_grouper')
var folders_assembler = require('./modules/sk_folders_assembler')
const { pid } = require('process')

global.modsys_root = path.resolve(__dirname, '../module_system/web/').split('\\').join('/');
global.modsys_roots = {
    node: modsys_root + '/node/',
    npm: modsys_root + '/npm/',
    sk: modsys_root + '/sk/'
}

global.web_core_root = path.resolve(__dirname, '../web/').split('\\').join('/');
global.web_core = {
    global_js_core: web_core_root + '/global_js_core/',
    soft_backend: web_core_root + '/soft_backend/'
}

global.soft_backend_root =  path.resolve(__dirname, '../../../soft_backend').split('\\').join('/');

global.bundleRoot = path.resolve(__dirname, '../../../.sk/bundle/').split('\\').join('/');

global.bundleDeepRoot = path.resolve(__dirname, '../../../.sk/bundle/deep/').split('\\').join('/');
global.deepGroupsRoot = path.resolve(__dirname, '../../../.sk/bundle/deep/groups/').split('\\').join('/');

global.bundleShallowRoot = path.resolve(__dirname, '../../../.sk/bundle/shallow/').split('\\').join('/');
global.shallowGroupsRoot = path.resolve(__dirname, '../../../.sk/bundle/shallow/groups/').split('\\').join('/');
global.shallowGroupsDataRoot = path.resolve(__dirname, '../../../.sk/bundle/shallow/groups/data/').split('\\').join('/');


var run = async ()=>{
    /****  First check if any of the files are locked an notify developer ***********/

    console.log(`Checking for locked files...`)
    
    var lockChecker = new (require(`./modules/LockChecker/lockChecker_${utils.getOS()}.js`))()

    await lockChecker.init()
    
    var lockedFiles = await lockChecker.checkFiles()

    var pidList = []
    
    if (lockedFiles.length > 0){
        for (var f of lockedFiles){
            pidList = [...pidList, ...f.procList.map(p=>p.pid)]
        }
        utils.reportError({keepAlive: true, msg: `[SK++ Bundler] Some files are locked/open in other applications or couldn't be found. Please close the applications locking these files by running the below command in an Administrator command prompt then try bundling again.`})
        utils.reportError({keepAlive: true, msg: `taskkill /PID ${[...new Set(pidList)].join(' /PID ')} /F`})

        for (var f of lockedFiles){
            if (f.status === 'not_found'){
                utils.reportError({keepAlive: true, msg: `[SK++ Bundler] File "${f.filename}" not found.`, file: f.path})
            } else {
                var procs = f.procList.map(p=>`${p.name} (PID ${p.pid})`).join(', ')
                utils.reportError({keepAlive: true, msg: `[SK++ Bundler]  File "${f.filename}" locked by: ${procs}`})
            }
        }

        process.exit(1)
    }

    /*******************/
    
    console.log(`Removing previous bundle...`)
    try { fs.rmSync(bundleRoot, {force: true, recursive: true}) } catch(err) {}

    console.log(`Creating bundle...`)
    console.log(` - Cleaning previous bundle...`)
    
    try { fs.rmSync(bundleRoot, { recursive: true, force: true }) } catch(err) {}
    fs.mkdirSync(bundleRoot, { recursive: true })

    fs.mkdirSync(bundleDeepRoot, { recursive: true })
    fs.mkdirSync(deepGroupsRoot, { recursive: true })

    fs.mkdirSync(bundleShallowRoot, { recursive: true })
    fs.mkdirSync(shallowGroupsRoot, { recursive: true })
    fs.mkdirSync(shallowGroupsDataRoot, { recursive: true })

    var src = path.resolve(__dirname + '/templates/sk_soft_backend_bundle_group_root_template.h')
    var dst = path.resolve(__dirname, '../../../.sk/bundle/bundle_group_root.h')
    fs.copyFileSync(src, dst)


    console.log(` - Listing files...`)

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


    console.log(` - Grouping files...`)
    var groupRes = grouper.forFiles({
        files: allEntries,
        groupSize: 0.3, // MB
    })

    console.log(` - Assembling folders...`)
    var foldersRes = folders_assembler.forFolders({
        folders: allEntries
    })

    console.log(` - Writing groups...`)

    var libraryTemplatePath =  __dirname + '/templates/sk_soft_backend_bundle_library_template.h'
    var libraryTemplate = fs.readFileSync(libraryTemplatePath).toString()
        .replace('<!deep_group_includes!>', groupRes.includesDef.deep)
        .replace('<!shallow_group_includes!>', groupRes.includesDef.shallow)
        .replace('<!groups!>', groupRes.groupsDefs + '\n')
        .replace('<!file_entries!>', groupRes.entriesDefs + '\n')
        .replace('<!folder_entries!>', foldersRes.join(',\n') + '\n')

    var libraryPath = path.resolve(__dirname, '../../../.sk/bundle/sk_soft_backend_bundle_library.h')
    fs.writeFileSync(libraryPath, libraryTemplate)

    console.log(`Finalizing...`)
    var filesToPermit = await utils.listFilesRecursive(bundleRoot, { followSymlinks: false })
    for (var i = 0; i < filesToPermit.length; i++) {
        var file = filesToPermit[i];
        try {
            fs.chmodSync(file, 0o666)
            fs.chownSync(file, process.getuid(), process.getgid());
        } catch(err) {
            console.warn(` - Warning: Could not set permissions for file ${file}. ${err.message}`)
        }
    }
    //ensuring all files have been written before exiting
    console.log(`Done!`)
}

run()