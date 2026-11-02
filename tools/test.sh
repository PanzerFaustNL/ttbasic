#!/bin/sh
set -eu
repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
test_dir=$(mktemp -d "${TMPDIR:-/tmp}/ttbasic-tests.XXXXXX")
trap 'rm -rf "$test_dir"' EXIT HUP INT TERM
if [ "${SANITIZE:-0}" = 1 ]; then
    sanitizer_flags='-fsanitize=address,undefined'
else
    sanitizer_flags=''
fi
# The .C extension means C++ to many host compilers: force ANSI C explicitly.
${CC:-cc} -x c -std=c89 -Wall -Wextra -Wno-deprecated-declarations \
    $sanitizer_flags -I "$repo_root/src" \
    "$repo_root/tests/test_objects.c" "$repo_root/src/BASIC.C" \
    -o "$test_dir/test_objects"
"$test_dir/test_objects" "$test_dir/MAIN.BAS"
