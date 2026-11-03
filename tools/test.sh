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
for suite in test_objects test_language test_numbers test_examples; do
    ${CC:-cc} -x c -std=c89 -Wall -Wextra -Wno-deprecated-declarations \
        $sanitizer_flags -I "$repo_root/src" \
        "$repo_root/tests/$suite.c" "$repo_root/src/BASIC.C" "$repo_root/src/MIDI.C" \
        -lm -o "$test_dir/$suite"
    if [ "$suite" = test_examples ]; then
        "$test_dir/$suite" "$repo_root/examples"
    else
        "$test_dir/$suite" "$test_dir/MAIN.BAS"
    fi
done

${CC:-cc} -x c -std=c89 -Wall -Wextra -Wno-deprecated-declarations \
    $sanitizer_flags -I "$repo_root/src" \
    "$repo_root/tests/test_project.c" "$repo_root/src/PROJECT.C" \
    -lm -o "$test_dir/test_project"
"$test_dir/test_project" "$test_dir"

${CC:-cc} -x c -std=c89 -Wall -Wextra -Wno-deprecated-declarations \
    $sanitizer_flags -I "$repo_root/src" \
    "$repo_root/tests/test_complete.c" "$repo_root/src/COMPLETE.C" \
    "$repo_root/src/PROJECT.C" -lm -o "$test_dir/test_complete"
"$test_dir/test_complete" "$test_dir"

${CC:-cc} -x c -std=c89 -Wall -Wextra -Wno-deprecated-declarations \
    $sanitizer_flags -I "$repo_root/src" \
    "$repo_root/tests/test_gui_debug.c" "$repo_root/src/BASIC.C" "$repo_root/src/MIDI.C" \
    -lm -o "$test_dir/test_gui_debug"
"$test_dir/test_gui_debug" "$repo_root/examples"

${CC:-cc} -x c -std=c89 -Wall -Wextra -Wno-deprecated-declarations \
    $sanitizer_flags -I "$repo_root/src" \
    "$repo_root/tests/test_form.c" "$repo_root/src/FORM.C" \
    "$repo_root/src/BASIC.C" "$repo_root/src/MIDI.C" -lm -o "$test_dir/test_form"
"$test_dir/test_form" "$repo_root/examples"

${CC:-cc} -x c -std=c89 -Wall -Wextra -Wno-deprecated-declarations \
    $sanitizer_flags -I "$repo_root/src" \
    "$repo_root/tests/test_midi.c" "$repo_root/src/BASIC.C" \
    "$repo_root/src/MIDI.C" -lm -o "$test_dir/test_midi"
"$test_dir/test_midi" "$repo_root/examples"
