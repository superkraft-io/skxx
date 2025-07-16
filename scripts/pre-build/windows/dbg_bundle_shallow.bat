@echo off
setlocal

echo [SK++ - Bundling: Shallow] Running prebuild script...
call node.exe "%SCRIPT_DIR%..\..\..\..\skxx\bundler\sk_prebuild_script.js" "shallow"

echo [SK++ - Bundling: Shallow] Running bundler...
call node.exe "%SCRIPT_DIR%..\..\..\..\skxx\bundler\sk_bundler.js"

endlocal
