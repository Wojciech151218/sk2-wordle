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

cmake -S "${project_dir}" -B "${build_dir}" -DCMAKE_BUILD_TYPE="${build_type}"
cmake --build "${build_dir}" --target sieci_server

if [[ ${#debugger[@]} -gt 0 ]]; then
    exec "${debugger[@]}" "${build_dir}/sieci_server" "$@"
else
    exec "${build_dir}/sieci_server" "$@"
fi
