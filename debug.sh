#!/usr/bin/env bash
set -euo pipefail

project_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
build_dir="${project_dir}/build/debug"

cmake -S "${project_dir}" -B "${build_dir}" -DCMAKE_BUILD_TYPE=Debug
cmake --build "${build_dir}"

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 <address> <port>"
    echo
    echo "Example:"
    echo "  $0 0.0.0.0 5000"
    exit 1
fi

if ! command -v gdb >/dev/null 2>&1; then
    echo "gdb is required to run this script."
    exit 1
fi

exec gdb --quiet --args "${build_dir}/sieci_server" "$1" "$2"


