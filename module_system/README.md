# Superkraft Module System

The SK Module System is a a NodeJS-like module system

There are a few modules available to use in your soft backend and your frontend views.


## SK Core Modules
The modules in the `sk` folder are modules developed by the Superkraft team with certain specific features.

These modules can me imported/required with or without the `sk:` prefix, e.g `sk:application` or `application`.

| Name       | Description       |        |
|----------------|----------------|----------------|
| `application`   | API for application information   |
| `vfs`   | Virtual File System module for file manipulation directly to memory rather than disk   |
| `web`   | Perform GET, POST and download files   |
| `debugMngr`   | Perform various debug related operations, such as programmatically open the dev tools of e.g the soft backend or the view   |
| `proton-js`   | ProtonJS is a substitute to ElectronJS   |  🟠 Partial    |


## Node.js Core Modules
The modules in the `node` folder reflect most if not all features of the native NodeJS modules.

These modules can me imported/required with or without the `node:` prefix, e.g `node:fs` or `fs`.

| Module            | Description                                                                 |   |
|--------------------|-----------------------------------------------------------------------------|---|
| `assert`          | Provides assertion functions for testing and validating conditions.         | -  |
| `async_hooks`     | Provides an API to track asynchronous resources in Node.js.                | -  |
| `buffer`          | Handles binary data, dealing with raw memory allocation.                   | -  |
| `child_process`   | Provides methods to spawn child processes.                                  | 🟠 Partial |
| `cluster`         | Enables the creation of multi-core applications.                           | -  |
| `console`         | Provides a simple debugging console similar to the browser's.              | -  |
| `crypto`          | Offers cryptographic functionality, including OpenSSL wrappers.            | -  |
| `dgram`           | Provides UDP datagram socket implementation.                               | -  |
| `diagnostics_channel` | A mechanism for context propagation in diagnostics.                   | -  |
| `dns`             | Provides DNS resolution functions.                                         | -  |
| `domain`          | Helps handle unhandled errors, but is deprecated.                          | -  |
| `events`          | Provides the EventEmitter class to handle events.                          | -  |
| `fs`              | Handles file system operations.                                            | 🟠 Partial  |
| `fs/promises`     | Promises-based file system API.                                             | -  |
| `http`            | Used to create HTTP servers and make HTTP requests.                        | -  |
| `http2`           | Provides support for HTTP/2.                                               | -  |
| `https`           | Used to create HTTPS servers and make HTTPS requests.                      | -  |
| `inspector`       | Provides an API for debugging and profiling.                               | -  |
| `module`          | Provides utilities for interacting with Node.js modules.                   | -  |
| `net`             | Creates network servers and clients.                                       | -  |
| `os`              | Provides information about the operating system.                           | 🟠 Partial  |
| `path`            | Handles and transforms file paths.                                         | 🟠 Partial  |
| `perf_hooks`      | Provides performance monitoring and metrics.                               | -  |
| `process`         | Provides information and control over the Node.js process.                | 🟠 Partial  |
| `punycode`        | Converts Unicode strings to Punycode and vice versa.                       | -  |
| `querystring`     | Parses and formats URL query strings.                                       | -  |
| `readline`        | Interfaces for reading input from a Readable stream, like process.stdin.   | -  |
| `repl`            | Provides a Read-Eval-Print-Loop (REPL) implementation.                     | -  |
| `stream`          | Handles streaming data.                                                    | -  |
| `stream/promises` | Promises-based stream API.                                                 | -  |
| `string_decoder`  | Decodes buffer to string.                                                  | -  |
| `sys`             | Alias for `util`. Deprecated.                                              | -  |
| `timers`          | Handles scheduling of functions (e.g., `setTimeout`, `setInterval`).       | -  |
| `timers/promises` | Promises-based timers API.                                                 | -  |
| `tls`             | Implements Transport Layer Security (TLS) and Secure Socket Layer (SSL).   | -  |
| `trace_events`    | Provides a mechanism to trace events.                                      | -  |
| `tty`             | Provides support for terminal interfaces.                                  | -  |
| `url`             | URL resolution and parsing utilities.                                      | -  |
| `util`            | Provides utilities for debugging and working with objects.                 | -  |
| `v8`              | Exposes APIs specific to the V8 JavaScript engine.                        | -  |
| `vm`              | Provides APIs to compile and run code within V8 contexts.                | -  |
| `wasi`            | WebAssembly System Interface.                                              | -  |
| `worker_threads`  | Enables multithreading in Node.js.                                         | -  |
| `zlib`            | Provides compression and decompression utilities.                         | -  |


## NPM Modules
The modules in the `npm` folder emulate a few specific NPM modules.


| Name       | Description       | Implementation       |
|----------------|----------------|----------------|
| `electron-js`   | See ProtonJS in the "SK Core Modules" section   |  ---   |
| `fs-extra`   | An extension of the `fs` module   | 🟠 Partial   |


#How to use

You can `require` or `import` any of the available models the same way you would in NodeJS.

For example, if you want to `require` the `fs` module, you simply do the following in any of your frontend code:
```javascript
const fs = require(`fs`)

const data_sync = fs.readFileSync("c:/myFile.txt");

const data_async = await fs.promises.readFile("c:/myFile.txt");
```


##Important to know

- NPM modules are not expected to work, but some may actually work
- Our module system is not a 1:1 implementation of the NodeJS module system, so don't expect it to work perfectly