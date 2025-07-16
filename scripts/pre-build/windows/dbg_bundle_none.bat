@echo off
setlocal

echo [SK++ - Bundling: Deep] Running prebuild script...
call node.exe "%SCRIPT_DIR%..\..\..\..\skxx\bundler\sk_prebuild_script.js" "none"

endlocal
