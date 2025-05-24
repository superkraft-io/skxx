const fs = require('fs')
const path = require('path')

var lister = require('./modules/sk_file_lister')
var grouper = require('./modules/sk_files_grouper')
var folders_assembler = require('./modules/sk_folders_assembler')

global.soft_backend_root =  path.resolve(__dirname, '../../project')
global.bundleRoot = path.resolve(__dirname, '../../sk_soft_backend_bundle/')
global.groupsRoot = path.resolve(__dirname, '../../sk_soft_backend_bundle/groups/')


fs.rmdirSync(bundleRoot, { recursive: true, force: true })
fs.mkdirSync(bundleRoot, { recursive: true })
fs.mkdirSync(groupsRoot, { recursive: true })

fs.copyFileSync(__dirname + '/templates/sk_soft_backend_bundle_entry_template.h', path.resolve(__dirname, '../../sk_soft_backend_bundle/sk_soft_backend_bundle_entry.h'))
fs.copyFileSync(__dirname + '/templates/sk_soft_backend_bundle_group_root_template.h', path.resolve(__dirname, '../../sk_soft_backend_bundle/sk_soft_backend_bundle_group_root.h'))



var allEntries = lister.listFiles(soft_backend_root)


var groupRes = grouper.forFiles({
    files: allEntries,
    groupSize: 0.3, // MB
})

var foldersRes = folders_assembler.forFolders({
    folders: allEntries
})

var libraryTemplatePath =  __dirname + '/templates/sk_soft_backend_bundle_library_template.h'
var libraryTemplate = fs.readFileSync(libraryTemplatePath).toString()
    .replace('<!group_includes!>', groupRes.includesDef)
    .replace('<!groups!>', groupRes.groupsDefs + '\n')
    .replace('<!file_entries!>', groupRes.entriesDefs + '\n')
    .replace('<!folder_entries!>', foldersRes.join(',\n') + '\n')

var libraryPath = path.resolve(__dirname, '../../sk_soft_backend_bundle/sk_soft_backend_bundle_library.h')
fs.writeFileSync(libraryPath, libraryTemplate)


process.exit()


var varsList = []

var entriesList = []

var varCounter = -1;



var dataEntryTemplatePath =  __dirname + '/templates/sk_soft_backend_bundle_data_entry_template.h'
var dataEntryTemplate = fs.readFileSync(dataEntryTemplatePath).toString()


var headerIncludes = []

function createEntryFile(id, info, data){
    var headerFilename = 'file_' + id + '.h'

    var entryFileData = dataEntryTemplate
    .replace('<!id!>', id)
    .replace('<!path!>', info.path)
    .replace('<!filename!>', info.filename)
    .replace('<!folders!>', info.folders.join(','))
    .replace('<!files!>', info.files.join(','))
    .replace('<!isFolder!>', info.isFolder)
    .replace('<!data!>', data.join(','))
    .split('<!size!>').join(data.length)


    var entryFilePath = filesRoot + headerFilename
    fs.writeFileSync(entryFilePath, entryFileData)

    headerIncludes.push(`#include "files/${headerFilename}"`)

    console.log(headerFilename + ' done!')
}

for (var i in allEntries) {
    var entry = allEntries[i]
    //var varLine = `    const unsigned char <!varName!>[<!bytes!>] = {<!data}!>};\n`
    
    //var entryLine = `        {"${entry.path}", new SK_SoftBackend_Bundle_Entry(${entry.isFolder}, "${entry.path}", "${entry.filename}", ${entry.size}, "${entry.folders.join(',')}", "${entry.files.join(',')}", static_cast<void*>(const_cast<unsigned char*>(<!data_pointer!>)))}`
    var entryLine = `        {"${entry.path}", new SK_SoftBackend_Bundle_Entry(${entry.isFolder}, "${entry.path}", "${entry.filename}", ${entry.size}, "${entry.folders.join(',')}", "${entry.files.join(',')}", static_cast<void*>(const_cast<unsigned char*>(<!data_pointer!>)))}`

    if (!entry.isFolder && entry.size > 0){
        varCounter++
        var varName = `data_${varCounter}`


        var fileData = fs.readFileSync(entry.originalPath);

        var lines = fileData.toString().split('\n');
        for (var u in lines) lines[u] = lines[u].trim();
        var fileData = lines.join('\n');
        const buffer = Buffer.from(fileData, 'utf8');

        createEntryFile(i, entry, buffer)

        /*varLine = varLine
            .replace('<!varName!>', varName)
            .replace('<!bytes!>', buffer.length)
            .replace('<!data}!>', buffer.join(','))
            */

        //varsList.push(varLine)

        //entryLine = entryLine.replace('<!data_pointer!>', varName)
        entriesList.push(entryLine)

    } else {
        entryLine = entryLine.replace('<!data_pointer!>', 'nullptr')
        entriesList.push(entryLine)
    }
}

console.log('Total file entries: ' + varsList.length)
console.log('Total folder entries: ' + entriesList.length)

console.log('Saving...')

fs.copyFileSync(__dirname + '/templates/sk_soft_backend_bundle_entry_template.h', path.resolve(__dirname, '../../sk_soft_backend_bundle/sk_soft_backend_bundle_entry.h'))
fs.copyFileSync(__dirname + '/templates/sk_soft_backend_bundle_class_template.h', path.resolve(__dirname, '../../sk_soft_backend_bundle/sk_soft_backend_bundle_class.h'))

var cppTemplatePath =  __dirname + '/templates/sk_soft_backend_bundle_data_template.h'
    fs.copyFileSync(__dirname + '/templates/sk_soft_backend_bundle_data_template.h', path.resolve(__dirname, '../../sk_soft_backend_bundle/sk_soft_backend_bundle_data.h'))

/*var cppTemplate = fs.readFileSync(cppTemplatePath).toString()


var cppTemplatePath =  path.resolve(__dirname, '../../sk_soft_backend_bundle/sk_soft_backend_bundle_data.h')
fs.writeFileSync(cppTemplatePath, cppTemplate.replace('<!data_vars!>', varsList.join('\n')).replace('<!entries!>', entriesList.join(',\n')))
*/

console.log(`Done!`)