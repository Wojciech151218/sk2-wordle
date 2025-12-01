# Debugging `sieci_server`

The project now ships with debug-friendly defaults and a helper script that
launches `gdb` with the correct arguments.

## Prerequisites

- `cmake` 3.16+
- `g++` (or any other compiler supported by CMake)
- `gdb`

## One-command debug session

```bash
./debug.sh <listen_address> <port>
```

Example:

```bash
./debug.sh 0.0.0.0 5000
```

What the script does:

1. Configures CMake with `-DCMAKE_BUILD_TYPE=Debug` into `build/debug`.
2. Builds the executable.
3. Starts `gdb --args build/debug/sieci_server <port> <address>`.

Inside `gdb` you can use the usual commands:

- `start` to run to `main`.
- `break <file>:<line>` to set breakpoints.
- `continue` to resume execution.
- `bt` to print a backtrace when paused or after a crash.

## Cursor / VS Code integrated debugging

This repo includes ready-to-use launch and task definitions under `.vscode/`.

Steps:

1. Open the folder in Cursor or VS Code.
2. Press `F5` (or pick **Run > Start Debugging**).
3. When prompted, enter the TCP port and listen address.

What happens:

- The `Build Debug` task runs (and chains to `Configure Debug Build` the first time), ensuring `build/debug/sieci_server` is up to date.
- The `Debug sieci_server` configuration launches `gdb` via the built-in `cppdbg` adapter. All breakpoints set in the editor will bind automatically.
- The launch configuration prompts for **address first, port second** to match `main.cpp`'s argument order.

Tips:

- Use the **Run and Debug** sidebar to change arguments later via the gear icon → `launch.json`.
- If you ever need to reconfigure, run the `Configure Debug Build` task manually from the command palette.

## Manual workflow

If you prefer running commands yourself:

```bash
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug
gdb --args build/debug/sieci_server <listen_address> <port>
```

The default build type in `CMakeLists.txt` is also `Debug`, so invoking CMake
without `-DCMAKE_BUILD_TYPE` will produce binaries with debug symbols.


