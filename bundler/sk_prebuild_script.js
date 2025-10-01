var start = async function(){
    var fs = require('fs')
    var path = require('path')
    const { argv } = require('process')
    var utils = require('./modules/sk_utils.js')

    console.log('[SK++ Bundler] Starting...')

    var args = utils.parseArgs(argv.slice(2))

    var accepted_modes = ['none', 'shallow', 'deep']

    var bundle_mode = args.config.split('-')[1].split('_')[0]
    var root_path = path.resolve(__dirname)
    var output_path = path.resolve(root_path + '../../../sk_target_build_defs.h')

    if (!accepted_modes.includes(bundle_mode)) throw `[SK++ Bundler] Invalid bundle mode. Requested ${bundle_mode}. Only accepts none, shallow and deep`

    var lines = []

    lines.push(`#define SK_BUNDLE_MODE_${bundle_mode.toUpperCase()} 1`)
    lines.push(`#define SK_BUNDLE_MODE "${bundle_mode}"`)

    if (bundle_mode === 'none'){
        console.log('[SK++ Bundler] No bundling needed')
    } else {
        lines.push('#define SK_ROUTE_FS_TO_BDFS 1')
        if (bundle_mode === 'shallow'){
            var shallow_data_path = path.resolve(root_path + '/../../sk_soft_backend_bundle/shallow/groups/data/')
            lines.push(`static std::string SK_BUNDLER_SHALLOW_DATA_PATH("${shallow_data_path.split('\\').join('/')}");`)
        }
        else if (bundle_mode === 'deep'){
            //no-op
        }

    }

    try { fs.rmSync(output_path, {force: true}) } catch(err) {}
    fs.writeFileSync(output_path, lines.join('\n'))
    fs.chmodSync(output_path, 600)


    //Run bundler if needed
    if (bundle_mode !== 'none'){
        await utils.runNode(path.resolve(__dirname, 'sk_bundler.js'), [`--bundle_mode`, bundle_mode])
    }

    console.log('[SK++ Bundler] Done')
}
start()