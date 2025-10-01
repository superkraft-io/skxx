const fs = require('fs');
const path = require('path');
var utils = require('../sk_utils.js')

global.SysInternals_Handle_Root = require('./lockChecker_root.js')

module.exports = class LockChecker_MacOS extends LockChecker_Root {

    async checkFileLocked(filePath){
        return new Promise((resolve, reject)=>{
           
        });
    }
}