const fs = require('fs');
const path = require('path');
const { spawn } = require('child_process');
const extract = require('extract-zip');

module.exports = {
    parseArgs(argv) {
        const out = {};
        for (let i = 0; i < argv.length; i++) {
            const a = argv[i];
            if (a.startsWith('--')) {
                const k = a.slice(2);
                const v = (i + 1 < argv.length && !argv[i + 1].startsWith('--')) ? argv[++i] : true;
                out[k] = v;
            }
        }
        return out;
    },
    
    runPs1(scriptPath, args = []){
        const shell = 'powershell.exe'

        return new Promise((resolve, reject) => {
            var fullArgs = [
                '-NoProfile',
                '-ExecutionPolicy', 'Bypass',
                '-File', scriptPath,
                ...args
            ]

            const child = spawn(shell, fullArgs, { windowsHide: true });

            let stdout = '', stderr = '';
            child.stdout.on('data', d => (stdout += d));
            child.stderr.on('data', d => (stderr += d));
            child.on('error', reject);
            child.on('close', code => resolve({ code, stdout: stdout.trim(), stderr: stderr.trim() }));
        });
    },

    runNode(script, args = []) {
        return new Promise((resolve, reject) => {
            const child = spawn(process.execPath, [path.resolve(script), ...args], {
                stdio: 'inherit', // or ['ignore','pipe','pipe'] to capture
            });
            child.on('error', reject);
            child.on('close', code => resolve(code));
        });
    },

    async unzipTo(zipFile, destDir) {
        const absZip = path.resolve(zipFile);
        const absDest = path.resolve(destDir);
        await fs.promises.mkdir(absDest, { recursive: true });

        // extract-zip overwrites existing files by default
        await extract(absZip, { dir: absDest });
    },

    async listFilesRecursive(rootDir, options = {}){
        const followSymlinks = !!options.followSymlinks;
        const maxDepth = Number.isFinite(options.maxDepth) ? options.maxDepth : Infinity;
        const filter = typeof options.filter === 'function' ? options.filter : () => true;

        const root = path.resolve(rootDir);
        const out = [];
        const stack = [{ dir: root, depth: 0 }];

        while (stack.length) {
            const { dir, depth } = stack.pop();
            let dh;
            try {
                dh = await fs.promises.opendir(dir);
            } catch {
                // Not a directory (or inaccessible) — if it’s a file, push it
                try {
                    const st = await fs.promises.stat(dir);
                    if (st.isFile()) out.push(path.resolve(dir));
                } catch {}
                continue;
            }

            for await (const dirent of dh) {
                const full = path.join(dir, dirent.name);
                if (!filter(full, dirent)) continue;

                if (dirent.isFile()) {
                    out.push(full);
                } else if (dirent.isDirectory()) {
                    if (depth < maxDepth) stack.push({ dir: full, depth: depth + 1 });
                } else if (dirent.isSymbolicLink() && followSymlinks) {
                    try {
                        const real = await fs.promises.realpath(full);
                        const st = await fs.promises.stat(real);
                        if (st.isFile()) out.push(real);
                        else if (st.isDirectory() && depth < maxDepth) {
                            stack.push({ dir: real, depth: depth + 1 });
                        }
                    } catch { /* ignore broken symlinks */ }
                }
            }
        }

        // Normalize & sort for stable output
        return out.map(p => path.resolve(p)).sort((a, b) => a.localeCompare(b));
    }
}