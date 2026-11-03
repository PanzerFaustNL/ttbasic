# TT BASIC 0.10

This is a development release of a bounded BASIC dialect inspired by Visual Basic. It includes a native GEM IDE, form designer, debugger and MIDI output. It is intended for testing and sharing with its documented limits; it does not implement the entire VB language.

Copy the supplied `TTBASIC` directory to drive C: and start `C:\TTBASIC\TTBASIC.PRG`. Use TT Medium or at least 640 × 480. The tested configuration is TOS 3.05, 8 MiB ST-RAM and 256 MiB TT-RAM under Hatari. No Pure C installation is needed to run this binary. The package includes the GPLv3 license, corresponding source, build files and NL/EN/DE manuals. It contains no TOS ROM or Pure C compiler/libraries.

Open `NUMBERS.TBP` for decimals, conversions, fractional loops and object array fields. `DESIGN.TBP` demonstrates the form designer. `DX7DEMO.TBP` starts with MIDI output disabled; its Arturia mode uses a configured Algorithm MIDI Learn mapping on CC18. Disable Arturia mode for original Yamaha parameter SysEx.

## Changes

- Single and Double values, decimal/scientific literals, floating-point division and exponentiation.
- Signed 32-bit Integer/Long aliases, checked assignments and integer-only API arguments.
- Explicit CInt/CLng/CDbl/CSng, Int/Fix and basic math functions.
- Fixed and dynamic typed array fields, direct `Me.field(index)` access and ReDim Preserve on fields.
- Completion and debugger support, numeric example and documentation in three languages.
- MIDI output and Yamaha DX7 / Arturia demonstration carried forward from development version 0.9.

## Migration

`/` now performs floating-point division; use `\` for integer division. Assigning fractions to Integer/Long requires an explicit conversion. CInt/CLng round ties to even; Fix truncates toward zero. Single and Double are reserved type names; rename methods named Double to DoubleValue. Keep a copy of old projects before migrating. Personal source files and IDE configuration are not automatically rewritten by the source installer.

## Known limits

One-dimensional arrays only, up to 128 elements and 32 container allocations per run. Up to 64 objects, 16 classes/interfaces, eight parameters and eight nested method calls. Strings are limited to 127 characters, output to 79 characters per line, and source files to 256 lines. GUI controls currently include Window, Button, Label, TextBox and CheckBox. BASIC file access, generic collections, overloads, garbage collection, exception handling and standalone BASIC compilation are not implemented. Floating-point arithmetic is approximate, not decimal Currency arithmetic. See the manuals for all limits and numeric ranges.

Physical TT and physical DX7 validation remain pending. The Arturia Algorithm mapping has been confirmed by the user; other Arturia parameter mappings are not yet configured. MIDI input and patch-bank transfers are not implemented.
