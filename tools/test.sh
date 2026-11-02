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
for suite in test_objects test_language test_examples; do
    ${CC:-cc} -x c -std=c89 -Wall -Wextra -Wno-deprecated-declarations \
        $sanitizer_flags -I "$repo_root/src" \
        "$repo_root/tests/$suite.c" "$repo_root/src/BASIC.C" \
        -o "$test_dir/$suite"
    if [ "$suite" = test_examples ]; then
        "$test_dir/$suite" "$repo_root/examples"
    else
        "$test_dir/$suite" "$test_dir/MAIN.BAS"
    fi
done

${CC:-cc} -x c -std=c89 -Wall -Wextra -Wno-deprecated-declarations \
    $sanitizer_flags -I "$repo_root/src" \
    "$repo_root/tests/test_project.c" "$repo_root/src/PROJECT.C" \
    -o "$test_dir/test_project"
"$test_dir/test_project" "$test_dir"
