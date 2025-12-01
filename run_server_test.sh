#!/usr/bin/env bash
set -euo pipefail

project_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
build_dir="${project_dir}/build"
build_type="${BUILD_TYPE:-Debug}"
debugger=()

if [[ "${1:-}" == "--debug" ]]; then
    debugger=(gdb --args)
    shift
fi

if [[ $# -lt 2 || $# -gt 3 ]]; then
    echo "Usage: $0 [--debug] <address> <port> [response_message]"
    exit 1
fi

cmake -S "${project_dir}" -B "${build_dir}" -DCMAKE_BUILD_TYPE="${build_type}"
cmake --build "${build_dir}" --target server_test

if [[ ${#debugger[@]} -gt 0 ]]; then
    exec "${debugger[@]}" "${build_dir}/server_test" "$@"
else
    exec "${build_dir}/server_test" "$@"
fi

