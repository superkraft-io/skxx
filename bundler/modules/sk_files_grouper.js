const fs = require('fs');
const path = require('path');


class FileGroup {
    constructor() {
        this.buffer = Buffer.alloc(0)
        this.files = []
        this.currentOffset = 0
    }

    static loadFileDataRaw(opt) {
        var removeWhitespace = opt.removeWhitespace || true

        var fileData = fs.readFileSync(opt.path);
        
        if (removeWhitespace){
            var lines = fileData.toString().split('\n');
            for (var u in lines) lines[u] = lines[u].trim();
            fileData = lines.join('\n');
        }

        return Buffer.from(fileData, 'utf8');
    }

    static loadFileData(opt){
        var fileTypesToMinify = ['.js', '.css', '.html', '.json', '.xml']
        var fileExt = path.extname(opt.path)
        var removeWhitespace = fileTypesToMinify.includes(fileExt.toLowerCase())
        
        return this.loadFileDataRaw({
            removeWhitespace: removeWhitespace,
            path: opt.path
        })
    }

    increaseSize(addedSize) {
        var newSize = this.buffer.length + addedSize
        if (newSize > this.buffer.length) {
            try {
                this.buffer = Buffer.concat([this.buffer, Buffer.allocUnsafe(addedSize)])
            } catch (e) {
                console.error('Error increasing buffer size:', e);
            }
        }
    }

    get size() {
        return this.buffer.length
    }

    addFile(opt) {
        this.files.push({
            idx: this.files.length,
            path: opt.path,
            size: opt.size,
            filename: opt.filename,
            offset: this.currentOffset
        })

        this.increaseSize(opt.size)

        var fileBuffer = FileGroup.loadFileData({path: opt.originalPath})
        fileBuffer.copy(this.buffer, this.currentOffset)
        this.currentOffset += opt.size
    }

    saveHeader() {
        var dataEntryTemplatePath =  __dirname + '/../templates/sk_soft_backend_bundle_group_template.h'
        var dataEntryTemplate = fs.readFileSync(dataEntryTemplatePath).toString()

        dataEntryTemplate = dataEntryTemplate
            .split('<!id!>').join(this.id)
            .replace('<!data_size!>', this.buffer.length)
            .replace('<!data!>', this.buffer.join(','))


        var offsets = []
        var sizes = []
        for (var i in this.files) {
            var file = this.files[i]
            
            offsets.push(file.offset)
            sizes.push(file.size)
        }
        
        dataEntryTemplate = dataEntryTemplate
            .replace('<!offsets_arr_size!>', this.files.length).replace('<!offsets!>', offsets.join(','))
            .replace('<!sizes_arr_size!>', this.files.length).replace('<!sizes!>', sizes.join(','))

        var savePath = groupsRoot + '/sk_soft_backend_bundle_group_' + this.id + '.h'
        fs.writeFileSync(savePath, dataEntryTemplate)

        this.headerDef = `#include "./groups/sk_soft_backend_bundle_group_${this.id}.h"`
        this.headerClass = `SK_SoftBackend_Bundle_Data_Group_${this.id}`
    }
}

global.FileGroup = FileGroup

module.exports = {
    forFiles: function(opt) {
        var totalSize = 0

        for (var i = 0; i < opt.files.length; i++) {
            var file = opt.files[i]
            if (file.isFolder) continue

            totalSize += file.size
        }


        var files = opt.files
        var groupSize = (opt.groupSize ? 1000 * 1000 * opt.groupSize : 1000 * 1000 * 10) // default 10MB

        var groups = []

        console.warn('GROUPING FOR LARGE FILES IS DISABLED. TURN IT BACK ON WHEN BUNDLER IS CONFIRMED TO WORK PROPERLY!')
        /*
        for (var i = files.length - 1; i >= 0; i--) {
            var file = files[i]
            if (file.size > groupSize) {
                var group = new FileGroup()
                group.addFile(file)
                groups.push(group)
                files.splice(i, 1)
            }
        }
            */

        var currentGroup = new FileGroup()
        for (var i = 0; i < files.length; i++) {
            var file = files[i]
            
            if (i > 3) break //temporary limit for testing

            if (file.isFolder) continue

            var expectedGroupSize = currentGroup.size + file.size
            if (expectedGroupSize > groupSize) {
                groups.push(currentGroup)
                currentGroup = new FileGroup()
            }

            currentGroup.addFile(file)
        }

        if (currentGroup.files.length > 0) groups.push(currentGroup)




        for (var i = 0; i < groups.length; i++) {
            var group = groups[i]
            group.id = i
            group.saveHeader()
        }

       
        

        var includeDefs = []
        var groupDefs = []
        var entriesDefs = []

        for (var groupIdx = 0; groupIdx < groups.length; groupIdx++) {
            var group = groups[groupIdx]

            includeDefs.push(group.headerDef)

            groupDefs.push(`        new ${group.headerClass}()`)

            for (var u in group.files) {
                var file = group.files[u]
                var entryLine = `       {"${file.path}", new SK_SoftBackend_Bundle_Entry_Info(${file.offset}, ${file.size}, ${groupIdx}, ${file.idx}, groups[${groupIdx}], false, "${file.filename}", "")}`
                entriesDefs.push(entryLine)
            }
        }

        var returnInfo = {
            includesDef: includeDefs.join('\n'),
            groupsDefs: groupDefs.join(',\n'),
            entriesDefs: entriesDefs.join(',\n')
        }
        
        return returnInfo
    }
};