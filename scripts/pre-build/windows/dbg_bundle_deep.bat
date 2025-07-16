@echo off
setlocal

echo [SK++ - Bundling: Deep] Running prebuild script...
call node.exe "%SCRIPT_DIR%..\..\..\..\skxx\bundler\sk_prebuild_script.js" "deep"

echo [SK++ - Bundling: Deep] Running bundler...
call node.exe "%SCRIPT_DIR%..\..\..\..\skxx\bundler\sk_bundler.js"

endlocal
