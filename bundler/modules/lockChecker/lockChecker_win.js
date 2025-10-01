const fs = require('fs');
const path = require('path');
var utils = require('../sk_utils.js')

global.LockChecker_Root = require('./lockChecker_root.js')

module.exports = class LockChecker_Win extends LockChecker_Root {
    async checkFileLocked(filePath){
        return new Promise(async (resolve, reject)=>{
            const script = path.resolve(__dirname, 'scripts/checklock.ps1').split('\\').join('/');
            const fileToCheck = filePath.split('\\').join('/');
            const { code, stdout, stderr } = await utils.runPs1(script, [fileToCheck]);

            if (stderr) {
                throw `Error checking file lock status: ${stderr}`
            }
            
            if (stdout === 'free') {
                return resolve({path: filePath, status: 'free'})
            }

            
            if (stdout === 'not_found') {
                return reject({path: filePath, status: 'not_found'})
            }

            
            var procList = []

            if (stdout.startsWith('locked,')) {
                const details = stdout.slice(7);
                var processes = details.split(',')

                for (let i in processes) {
                    var proc = processes[i]
                    var split = proc.split(':')
                    procList.push({pid: split[0], name: split[1] })
                }
                reject({path: filePath, procList: procList})
            }
            
        });
    }
}