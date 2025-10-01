//!!!!!    TODO: Also bundle all modsys web files, not just the soft backend files.    !!!!


const fs = require('fs')
const path = require('path')
const os = require('os')

var currentOS = os.platform()
if (currentOS === 'win32') currentOS = 'win'
else if (currentOS === 'darwin') currentOS = 'macos'
else if (currentOS === 'linux') currentOS = 'linux'
else throw new Error(`Unsupported OS: ${currentOS}`)

const { argv } = require('process')


var utils = require('./modules/sk_utils.js')
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

global.soft_backend_root =  path.resolve(__dirname, '../../project').split('\\').join('/');

global.bundleRoot = path.resolve(__dirname, '../../sk_soft_backend_bundle/').split('\\').join('/');

global.bundleDeepRoot = path.resolve(__dirname, '../../sk_soft_backend_bundle/deep/').split('\\').join('/');
global.deepGroupsRoot = path.resolve(__dirname, '../../sk_soft_backend_bundle/deep/groups/').split('\\').join('/');

global.bundleShallowRoot = path.resolve(__dirname, '../../sk_soft_backend_bundle/shallow/').split('\\').join('/');
global.shallowGroupsRoot = path.resolve(__dirname, '../../sk_soft_backend_bundle/shallow/groups/').split('\\').join('/');
global.shallowGroupsDataRoot = path.resolve(__dirname, '../../sk_soft_backend_bundle/shallow/groups/data/').split('\\').join('/');


var run = async ()=>{
    /****  First check if any of the files are locked an notify developer ***********/

    console.log(`Checking for locked files...`)
    
    var lockChecker = new (require(`./modules/LockChecker/lockChecker_${currentOS}.js`))()
    await lockChecker.init()
    var lockedFiles = await lockChecker.checkFiles()

    var pidList = []

    if (lockedFiles.length > 0){
        console.error(`[ERROR] Some files are locked/open in other applications or couldn't be found:`)
        for (var f of lockedFiles){

            pidList = [...pidList, ...f.procList.map(p=>p.pid)]

            if (f.status === 'not_found'){
                console.error(` - File not found: ${f.path}`)
            } else {
                var procs = f.procList.map(p=>`${p.name} (PID ${p.pid})`).join(', ')
                console.error(` - File locked: ${f.path}  (locked by: ${procs})`)
            }
        }


        console.log(`\nPlease close the applications locking these files by running the following commands in an Administrator command prompt:` +
        `\n\n    taskkill /PID ${[...new Set(pidList)].join(' /PID ')} /F\n` +
        `\nThen try bundling again.\n`  )

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

    fs.copyFileSync(__dirname + '/templates/sk_soft_backend_bundle_group_root_template.h', path.resolve(__dirname, '../../sk_soft_backend_bundle/sk_soft_backend_bundle_group_root.h'))


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

    var libraryPath = path.resolve(__dirname, '../../sk_soft_backend_bundle/sk_soft_backend_bundle_library.h')
    fs.writeFileSync(libraryPath, libraryTemplate)

    console.log(`Finalizing...`)


    //ensuring all files have been written before exiting
    setTimeout(()=>{
        console.log(`Done!`)
    }, 2000)
}

run()